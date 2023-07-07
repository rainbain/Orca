{
description = "Orca Nix Enviroment";

inputs = {
    nixpkgs.url = "nixpkgs/nixos-23.05";
};

outputs = {self, nixpkgs}: {
    packages."x86_64-linux" = let
        pkgs = import nixpkgs {
            system = "x86_64-linux";
        };
    in {
        default = pkgs.stdenv.mkDerivation {
            name = "orca_env";
            src = ./.;

            nativeBuildInputs = [
                pkgs.verilator
                pkgs.cmake
            ];

            buildInputs = [
            ];
        };
    };
};
}