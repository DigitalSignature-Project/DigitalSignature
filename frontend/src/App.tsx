import type { JSX } from "react";
import { useState } from "react";

import { RsaBtn, RsaParallelBtn } from "./components/BmiButton";
import { BmiHeader } from "./components/BmiHeader";
import { BmiResult } from "./components/BmiResult";
import { BmiInputs } from "./components/BmiInputs";

import { calculateRsa, calculateRsaParallel } from "./services/rsaAPI";

export const App = (): JSX.Element => {
  const [clicked, setClicked] = useState<boolean>(false);
  const [bmi, setBmi] = useState<string>("BMI");
  const [bits, setBits] = useState<number>(1024);
  const threads: number = 4;

  const calculateRsaHandler = async () => {
    setClicked(!clicked);
    const data = await calculateRsa(bits);
    setBmi(`Na jednym watku: ${data.key_pub}`);
  };

  const calculateRsaParallelHandler = async () => {
    const data = await calculateRsaParallel(bits, threads);
    setBmi(`Na 4 watkach: ${data.key_pub}`);
  };

  return (
    <>
      <BmiHeader />
      <BmiInputs bits={bits} setBits={setBits} />
      <RsaBtn onClick={calculateRsaHandler} clicked={clicked} />
      <RsaParallelBtn onClick={calculateRsaParallelHandler} />
      <BmiResult bmi={bmi} />
    </>
  );
};
