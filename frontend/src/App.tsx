import type { JSX } from "react";
import { useState } from "react";

import { BmiButton } from "./components/BmiButton";
import { BmiHeader } from "./components/BmiHeader";
import { BmiResult } from "./components/BmiResult";
import { BmiInputs } from "./components/BmiInputs";

export const App = (): JSX.Element => {
  const [clicked, setClicked] = useState<boolean>(false);
  const [bmi, setBmi] = useState<string>("BMI");
  const [bits, setBits] = useState<number | string>("0");

  const calculateBmi = async () => {
    setClicked(!clicked);

    const response = await fetch(
      "http://127.0.0.1:8000/api/rsa_generate_keys",
      {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          bits: Number(bits),
        }),
      },
    );

    const data: { key_pub: string; key_priv: string; key_module: string } =
      await response.json();
    setBmi(`BMI: ${data.key_pub}`);
  };

  return (
    <>
      <BmiHeader />
      <BmiInputs bits={bits} setBits={setBits} />
      <BmiButton onClick={calculateBmi} clicked={clicked} />
      <BmiResult bmi={bmi} />
    </>
  );
};
