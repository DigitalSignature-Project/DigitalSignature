import { type JSX } from "react";

type RsaBtnProps = {
  onClick: () => void;
  clicked: boolean;
};

type RsaParallelBtnProps = {
  onClick: () => void;
};

export const RsaBtn = ({ onClick, clicked }: RsaBtnProps): JSX.Element => {
  return (
    <p>
      <button onClick={onClick} className="btn btn-primary">
        {clicked ? "Calculate RSA" : "Calculate Rsa"}
      </button>
    </p>
  );
};

export const RsaParallelBtn = ({
  onClick,
}: RsaParallelBtnProps): JSX.Element => {
  return (
    <p>
      <button onClick={onClick} className="btn btn-primary">
        Calcualte Rsa Parallel
      </button>
    </p>
  );
};
