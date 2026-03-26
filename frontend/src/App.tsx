import type { JSX } from "react";
import { useState } from "react";

import { BmiButton } from "./components/BmiButton";
import { BmiHeader } from "./components/BmiHeader";
import { BmiResult } from "./components/BmiResult";
import { BmiInputs } from "./components/BmiInputs";

export const App = (): JSX.Element => {
  const [clicked, setClicked] = useState<boolean>(false);
  const [bmi, setBmi] = useState<string>("BMI");
  const [height, setHeight] = useState<number | string>("0");
  const [weight, setWeight] = useState<number | string>("0");

  const calculateBmi = async () => {
    setClicked(!clicked);

    const response = await fetch("http://127.0.0.1:8000/api/bmi", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ height: Number(height), weight: Number(weight) }),
    });

    const data: { result: number } = await response.json();
    setBmi(`BMI: ${data.result.toFixed(1)}`);
  };

  return (
    <>
      <BmiHeader />
      <BmiInputs
        height={height}
        weight={weight}
        setHeight={setHeight}
        setWeight={setWeight}
      />
      <BmiButton onClick={calculateBmi} clicked={clicked} />
      <BmiResult bmi={bmi} />
    </>
  );
};
