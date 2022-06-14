---
sidebar_position: 7
title: List
---

Squiggle lists are a lot like Python lists or Ruby arrays. They accept all types.

```javascript
myList = [3, normal(5, 2), "random"];
```

### make

**Note: currently just called `makeList`, without the preix**

```
List.make: (number, 'a) => list<'a>
```

Returns an array of size `n` filled with the value.

```js
List.make(4, 1); // creates the list [1, 1, 1, 1]
```

See [Rescript implementation](https://rescript-lang.org/docs/manual/latest/api/belt/array#make)

### toString

```
toString: (list<'a>) => string
```

### length

```
length: (list<'a>) => number
```

### up to

**Note: currently just called `upTo`, without the preix**

```
List.upTo: (low:number, high:number) => list<number>
```

```js
List.upTo(0, 5); // creates the list [0, 1, 2, 3, 4, 5]
```

Syntax taken from [Ruby](https://apidock.com/ruby/v2_5_5/Integer/upto).

### first

```
first: (list<'a>) => 'a
```

### last

```
last: (list<'a>) => 'a
```

### reverse

```
reverse: (list<'a>) => list<'a>
```

### map

```
map: (list<'a>, a => b) => list<'b>
```

See [Rescript implementation](https://rescript-lang.org/docs/manual/latest/api/belt/array#map).

### reduce

```
reduce: (list<'b>, 'a, ('a, 'b) => 'a) => 'a
```

`reduce(arr, init, f)`

Applies `f` to each element of `arr`. The function `f` has two paramaters, an accumulator and the next value from the array.

```js
reduce([2, 3, 4], 1, {|acc, value| acc + value}) == 10
```

See [Rescript implementation](https://rescript-lang.org/docs/manual/latest/api/belt/array#reduce).

### reduce reverse

```
reduceReverse: (list<'b>, 'a, ('a, 'b) => 'a) => 'a
```

Works like `reduce`, but the function is applied to each item from the last back to the first.

See [Rescript implementation](https://rescript-lang.org/docs/manual/latest/api/belt/array#reducereverse).