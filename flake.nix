{
  inputs = {
    nixpkgs.url = github:nixos/nixpkgs/nixos-23.05;
    flake-utils.url = github:numtide/flake-utils;
  };

  outputs = { self, nixpkgs, flake-utils, ... }: flake-utils.lib.eachDefaultSystem (currentSystem:
    let
      pkgs = import nixpkgs {
        system = currentSystem;
      };
    in with pkgs; {
      devShell = let
        py-pkgs = ps: with ps; [ ];
      in mkShell rec {
        nativeBuildInputs = [
          cmake
          gnumake
          gcc
          qt6.full
          qtcreator
          (python3.withPackages py-pkgs)
        ];

        LD_LIBRARY_PATH = "${lib.makeLibraryPath nativeBuildInputs}";
      };

      packages.default = mkDerivation {};
    }
  );
}
