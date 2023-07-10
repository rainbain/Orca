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
                pkgs.libffi
                pkgs.libedit
                pkgs.z3
                pkgs.libbsd
                pkgs.llvm
                pkgs.gtkwave
            ];

            buildInputs = [
                pkgs.udev
                pkgs.libevdev
                pkgs.qt6.full
                pkgs.fmt
                pkgs.pugixml
                pkgs.enet
                pkgs.xxHash
                pkgs.bzip2
                pkgs.zstd
                pkgs.minizip
                pkgs.lzo
                pkgs.libspng
                pkgs.libusb
                pkgs.sfml
                pkgs.miniupnpc
                pkgs.mbedtls
                pkgs.curl
                pkgs.discord-rpc
                pkgs.gtest
                pkgs.hidapi
                pkgs.xorg.libX11
                pkgs.xorg.libX11.dev
                pkgs.xorg.libxcb.dev
                pkgs.cubeb
                pkgs.xorg.libXi
                pkgs.xorg.libXrandr
                pkgs.libffi
                pkgs.libedit
                pkgs.z3
                pkgs.libbsd
                pkgs.llvm
            ];
        };
    };
};
}