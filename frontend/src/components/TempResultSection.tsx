import { type JSX } from "react";

type TempResultSectionProps = {
  data: string;
};

export const TempResultSection = ({
  data,
}: TempResultSectionProps): JSX.Element => {
  return <span className="font-medium text-lg text-black">{data}</span>;
};
