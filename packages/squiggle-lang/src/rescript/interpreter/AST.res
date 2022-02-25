open ASTTypes

let toString = ASTTypes.Node.toString

let envs = (samplingInputs, environment) => {
  samplingInputs: samplingInputs,
  environment: environment,
  evaluateNode: ASTEvaluator.toLeaf,
}

let toLeaf = (samplingInputs, environment, node: node) =>
  ASTEvaluator.toLeaf(envs(samplingInputs, environment), node)

let toPointSetDist = (samplingInputs, environment, node: node) =>
  switch toLeaf(samplingInputs, environment, node) {
  | Ok(#RenderedDist(pointSetDist)) => Ok(pointSetDist)
  | Ok(_) => Error("Rendering failed.")
  | Error(e) => Error(e)
  }

let runFunction = (samplingInputs, environment, inputs, fn: ASTTypes.Function.t) => {
  let params = envs(samplingInputs, environment)
  ASTTypes.Function.run(params, inputs, fn)
}