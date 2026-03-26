import { type JSX } from "react";

type BmiButtonProps = {
  onClick: () => void;
  clicked: boolean;
};

export const BmiButton = ({
  onClick,
  clicked,
}: BmiButtonProps): JSX.Element => {
  return (
    <p>
      <button onClick={onClick} className="btn btn-primary">
        {clicked ? "Calculate BMI" : "Calculate Bmi"}
      </button>
    </p>
  );
};
