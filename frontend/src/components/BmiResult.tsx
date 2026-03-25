import type { JSX } from "react";

export const BmiResult = ({ bmi }: { bmi: string }): JSX.Element => {
  return (
    <p className="text-3xl font-bold text-primary" id="bmi-result">
      {bmi}:
    </p>
  );
};
