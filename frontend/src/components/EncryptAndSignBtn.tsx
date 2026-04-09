import { type JSX } from "react";

type EncryptAndSignBtnProps = {
  onClick: () => void;
};

export const EncryptAndSignBtn = ({
  onClick,
}: EncryptAndSignBtnProps): JSX.Element => {
  return (
    <button
      className="
    bg-[#1e40af] hover:bg-[#1e3a8a]
    active:scale-95
    active:shadow-sm
    text-white px-8 py-3 rounded-xl font-semibold
    shadow-md transition-all duration-150
    cursor-pointer mt-6
  "
      onClick={onClick}
    >
      Encrypt and Sign
    </button>
  );
};
