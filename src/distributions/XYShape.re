open DistTypes;

type t = xyShape;

let toJs = (t: t) => {
  {"xs": t.xs, "ys": t.ys};
};
let minX = (t: t) => t.xs |> E.A.first;
let maxX = (t: t) => t.xs |> E.A.last;
let first = (t: t) =>
  switch (t.xs |> E.A.first, t.ys |> E.A.first) {
  | (Some(x), Some(y)) => Some((x, y))
  | _ => None
  };
let last = (t: t) =>
  switch (t.xs |> E.A.last, t.ys |> E.A.last) {
  | (Some(x), Some(y)) => Some((x, y))
  | _ => None
  };

let unsafeFirst = (t: t) => first(t) |> E.O.toExn("Unsafe operation");
let unsafeLast = (t: t) => last(t) |> E.O.toExn("Unsafe operation");

let zip = t => Belt.Array.zip(t.xs, t.ys);
let getBy = (t: t, fn) => t |> zip |> Belt.Array.getBy(_, fn);

let firstPairAtOrBeforeValue = (xValue, t: t) => {
  let zipped = zip(t);
  let firstIndex =
    zipped |> Belt.Array.getIndexBy(_, ((x, y)) => x > xValue);
  let previousIndex =
    switch (firstIndex) {
    | None => Some(Array.length(zipped) - 1)
    | Some(0) => None
    | Some(n) => Some(n - 1)
    };
  previousIndex |> Belt.Option.flatMap(_, Belt.Array.get(zipped));
};

module XtoY = {
  let ifAtX = (f, t: t) =>
    getBy(t, ((x, _)) => x == f) |> E.O.fmap(((_, y)) => y);

  let stepwise = (f, t: t) =>
    firstPairAtOrBeforeValue(f, t) |> E.O.fmap(((_, y)) => y);

  // TODO: When Roman's PR comes in, fix this bit. This depends on interpolation, obviously.
  let linear = (f, t: t) => t |> CdfLibrary.Distribution.findY(f);
};

let pointwiseMap = (fn, t: t): t => {xs: t.xs, ys: t.ys |> E.A.fmap(fn)};
let fromArray = ((xs, ys)): t => {xs, ys};
let fromArrays = (xs, ys): t => {xs, ys};

// todo: maybe not needed?
// let comparePoint = (a: float, b: float) => a > b ? 1 : (-1);

let comparePoints = ((x1: float, y1: float), (x2: float, y2: float)) =>
  switch (x1 == x2, y1 == y2) {
  | (false, _) => compare(x1, x2)
  | (true, false) => compare(y1, y2)
  | (true, true) => (-1)
  };

// todo: This is broken :(
let combine = (t1: t, t2: t) => {
  let totalLength = E.A.length(t1.xs) + E.A.length(t2.xs);
  let array = Belt.Array.concat(zip(t1), zip(t2));
  Array.sort(comparePoints, array);
  array |> Belt.Array.unzip |> fromArray;
};

let intersperce = (t1: t, t2: t) => {
  let items: ref(array((float, float))) = ref([||]);
  let t1 = zip(t1);
  let t2 = zip(t2);

  Belt.Array.forEachWithIndex(t1, (i, item) => {
    switch (Belt.Array.get(t2, i)) {
    | Some(r) => items := E.A.append(items^, [|item, r|])
    | None => items := E.A.append(items^, [|item|])
    }
  });
  items^ |> Belt.Array.unzip |> fromArray;
};

let yFold = (fn, t: t) => {
  E.A.fold_left(fn, 0., t.ys);
};

let ySum = yFold((a, b) => a +. b);

let _transverse = fn =>
  Belt.Array.reduce(_, [||], (items, (x, y)) =>
    switch (E.A.last(items)) {
    | Some((_, yLast)) => Belt.Array.concat(items, [|(x, fn(y, yLast))|])
    | None => [|(x, y)|]
    }
  );

let _transverseShape = (fn, p: t) => {
  Belt.Array.zip(p.xs, p.ys)
  |> _transverse(fn)
  |> Belt.Array.unzip
  |> fromArray;
};

let accumulateYs = _transverseShape((aCurrent, aLast) => aCurrent +. aLast);
let subtractYs = _transverseShape((aCurrent, aLast) => aCurrent -. aLast);

let findY = CdfLibrary.Distribution.findY;
let findX = CdfLibrary.Distribution.findX;

// I'm really not sure this part is actually what we want at this point.
module Range = {
  // ((lastX, lastY), (nextX, nextY))
  type zippedRange = ((float, float), (float, float));

  let floatSum = Belt.Array.reduce(_, 0., (a, b) => a +. b);
  let toT = r => r |> Belt.Array.unzip |> fromArray;
  let nextX = ((_, (nextX, _)): zippedRange) => nextX;

  let rangePointAssumingSteps =
      (((lastX, lastY), (nextX, nextY)): zippedRange) => (
    nextX,
    lastY,
  );

  let rangeAreaAssumingTriangles =
      (((lastX, lastY), (nextX, nextY)): zippedRange) =>
    (nextX -. lastX) *. (lastY +. nextY) /. 2.;

  let delta_y_over_delta_x =
      (((lastX, lastY), (nextX, nextY)): zippedRange) =>
    (nextY -. lastY) /. (nextX -. lastX);

  let mapYsBasedOnRanges = (fn, t) =>
    Belt.Array.zip(t.xs, t.ys)
    |> E.A.toRanges
    |> E.R.toOption
    |> E.O.fmap(r => r |> Belt.Array.map(_, r => (nextX(r), fn(r))));

  let integrateWithTriangles = z => {
    let rangeItems = mapYsBasedOnRanges(rangeAreaAssumingTriangles, z);
    (
      switch (rangeItems, z |> first) {
      | (Some(r), Some((firstX, _))) =>
        Some(Belt.Array.concat([|(firstX, 0.0)|], r))
      | _ => None
      }
    )
    |> E.O.fmap(toT)
    |> E.O.fmap(accumulateYs);
  };

  let derivative = mapYsBasedOnRanges(delta_y_over_delta_x);

  let stepsToContinuous = t =>
    Belt.Array.zip(t.xs, t.ys)
    |> E.A.toRanges
    |> E.R.toOption
    |> E.O.fmap(r => r |> Belt.Array.map(_, rangePointAssumingSteps))
    |> E.O.fmap(Belt.Array.unzip)
    |> E.O.fmap(fromArray)
    |> E.O.fmap(intersperce(t));
};