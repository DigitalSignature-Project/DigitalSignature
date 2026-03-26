import type { JSX } from "react";

type BmiInputsProps = {
  height: number | string;
  weight: number | string;
  setHeight: (h: number) => void;
  setWeight: (w: number) => void;
};

export const BmiInputs = ({
  height,
  weight,
  setHeight,
  setWeight,
}: BmiInputsProps): JSX.Element => {
  return (
    <p>
      <input
        type="number"
        name="height-inp"
        id="height-inp"
        value={height}
        onChange={(e) => setHeight(Number(e.target.value))}
        className="input input-primary mb-1"
      />
      <input
        type="number"
        name="weight-inp"
        id="weight-inp"
        value={weight}
        onChange={(e) => setWeight(Number(e.target.value))}
        className="input input-primary mb-1"
      />
    </p>
  );
};
