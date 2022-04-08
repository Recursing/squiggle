type genericDist =
  | PointSet(PointSetTypes.pointSetDist)
  | SampleSet(SampleSet.t)
  | Symbolic(SymbolicDistTypes.symbolicDist)

type error =
  | NotYetImplemented
  | Unreachable
  | DistributionVerticalShiftIsInvalid
  | Other(string)

module Operation = {
  type direction =
    | Algebraic
    | Pointwise

  type arithmeticOperation = [
    | #Add
    | #Multiply
    | #Subtract
    | #Divide
    | #Exponentiate
    | #Logarithm
  ]

  let arithmeticToFn = (arithmetic: arithmeticOperation) =>
    switch arithmetic {
    | #Add => \"+."
    | #Multiply => \"*."
    | #Subtract => \"-."
    | #Exponentiate => \"**"
    | #Divide => \"/."
    | #Logarithm => (a, b) => log(a) /. log(b)
    }

  type toFloat = [
    | #Cdf(float)
    | #Inv(float)
    | #Mean
    | #Pdf(float)
    | #Sample
  ]

  type pointsetXSelection = [#Linear | #ByWeight]

  type toDist =
    | Normalize
    | ToPointSet
    | ToSampleSet(int)
    | Truncate(option<float>, option<float>)
    | Inspect

  type toFloatArray = Sample(int)

  type toString = 
  | ToString
  | ToSparkline(int)

  type fromDist =
    | ToFloat(toFloat)
    | ToDist(toDist)
    | ToDistCombination(direction, arithmeticOperation, [#Dist(genericDist) | #Float(float)])
    | ToString(toString)

  type singleParamaterFunction =
    | FromDist(fromDist)
    | FromFloat(fromDist)

  type genericFunctionCallInfo =
    | FromDist(fromDist, genericDist)
    | FromFloat(fromDist, float)
    | Mixture(array<(genericDist, float)>)

  let distCallToString = (distFunction: fromDist): string =>
    switch distFunction {
    | ToFloat(#Cdf(r)) => `cdf(${E.Float.toFixed(r)})`
    | ToFloat(#Inv(r)) => `inv(${E.Float.toFixed(r)})`
    | ToFloat(#Mean) => `mean`
    | ToFloat(#Pdf(r)) => `pdf(${E.Float.toFixed(r)})`
    | ToFloat(#Sample) => `sample`
    | ToDist(Normalize) => `normalize`
    | ToDist(ToPointSet) => `toPointSet`
    | ToDist(ToSampleSet(r)) => `toSampleSet(${E.I.toString(r)})`
    | ToDist(Truncate(_, _)) => `truncate`
    | ToDist(Inspect) => `inspect`
    | ToString(ToString) => `toString`
    | ToString(ToSparkline(n)) => `toSparkline(${E.I.toString(n)})`
    | ToDistCombination(Algebraic, _, _) => `algebraic`
    | ToDistCombination(Pointwise, _, _) => `pointwise`
    }

  let toString = (d: genericFunctionCallInfo): string =>
    switch d {
    | FromDist(f, _) | FromFloat(f, _) => distCallToString(f)
    | Mixture(_) => `mixture`
    }
}