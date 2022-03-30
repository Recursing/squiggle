type errorValue =
  | REArrayIndexNotFound(string, int)
  | REFunctionExpected(string)
  | REJs(option<string>, option<string>) // Javascript Exception
  | RERecordPropertyNotFound(string, string)
  | RETodo(string) // To do

let showError = err =>
  switch err {
  | REArrayIndexNotFound(msg, index) => `${msg}: ${Js.String.make(index)}`
  | REFunctionExpected(msg) => `Function expected: ${msg}`
  | REJs(omsg, oname) => {
      let answer = "JS Exception:"
      let answer = switch oname {
      | Some(name) => `${answer} ${name}`
      | _ => answer
      }
      let answer = switch omsg {
      | Some(msg) => `${answer}: ${msg}`
      | _ => answer
      }
      answer
    }
  | RERecordPropertyNotFound(msg, index) => `${msg}: ${index}`
  | RETodo(msg) => `TODO: ${msg}`
  }