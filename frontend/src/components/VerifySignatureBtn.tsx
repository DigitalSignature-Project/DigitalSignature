import { type JSX } from "react";

type VerifySignatureBtnProps = { onClick: () => void };

export const VerifySignatureBtn = ({
  onClick,
}: VerifySignatureBtnProps): JSX.Element => {
  return (
    <button
      type="button"
      className="
    bg-[#1e40af] hover:bg-[#1e3a8a]
    active:scale-95
    active:shadow-sm
    text-white px-8 py-3 rounded-xl font-semibold
    shadow-md transition-all duration-150
    cursor-pointer mt-8
  "
      onClick={onClick}
    >
      Verify signature
    </button>
  );
};
