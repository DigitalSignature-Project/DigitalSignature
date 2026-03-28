import type { JSX } from "react";

type BmiInputsProps = {
  bits: number | string;
  setBits: (w: number) => void;
};

export const BmiInputs = ({ bits, setBits }: BmiInputsProps): JSX.Element => {
  return (
    <p>
      <input
        type="number"
        name="height-inp"
        id="height-inp"
        value={bits}
        onChange={(e) => setBits(Number(e.target.value))}
        className="input input-primary mb-1"
      />
    </p>
  );
};
