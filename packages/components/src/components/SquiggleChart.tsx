import * as React from "react";
import {
  squiggleExpression,
  bindings,
  environment,
  jsImports,
  defaultImports,
  defaultBindings,
  defaultEnvironment,
} from "@quri/squiggle-lang";
import { FunctionChartSettings } from "./FunctionChart";
import { useSquiggle } from "../lib/hooks";
import { SquiggleErrorAlert } from "./SquiggleErrorAlert";
import { SquiggleItem } from "./SquiggleItem";

export interface SquiggleChartProps {
  /** The input string for squiggle */
  squiggleString?: string;
  /** If the output requires monte carlo sampling, the amount of samples */
  sampleCount?: number;
  /** The amount of points returned to draw the distribution */
  environment?: environment;
  /** If the result is a function, where the function starts, ends and the amount of stops */
  chartSettings?: FunctionChartSettings;
  /** When the squiggle code gets reevaluated */
  onChange?(expr: squiggleExpression | undefined): void;
  /** CSS width of the element */
  width?: number;
  height?: number;
  /** Bindings of previous variables declared */
  bindings?: bindings;
  /** JS imported parameters */
  jsImports?: jsImports;
  /** Whether to show a summary of the distribution */
  showSummary?: boolean;
  /** Whether to show type information about returns, default false */
  showTypes?: boolean;
  /** Whether to show graph controls (scale etc)*/
  showControls?: boolean;
}

const defaultOnChange = () => {};
const defaultChartSettings = { start: 0, stop: 10, count: 20 };

export const SquiggleChart: React.FC<SquiggleChartProps> = ({
  squiggleString = "",
  environment,
  onChange = defaultOnChange, // defaultOnChange must be constant, don't move its definition here
  height = 200,
  bindings = defaultBindings,
  jsImports = defaultImports,
  showSummary = false,
  width,
  showTypes = false,
  showControls = false,
  chartSettings = defaultChartSettings,
}) => {
  const { result } = useSquiggle({
    code: squiggleString,
    bindings,
    environment,
    jsImports,
    onChange,
  });

  if (result.tag !== "Ok") {
    return <SquiggleErrorAlert error={result.value} />;
  }

  return (
    <SquiggleItem
      expression={result.value}
      width={width}
      height={height}
      showSummary={showSummary}
      showTypes={showTypes}
      showControls={showControls}
      chartSettings={chartSettings}
      environment={environment ?? defaultEnvironment}
    />
  );
};
