{
  description = "A developer environment for verte.";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    systems.url = "github:nix-systems/default";
    devenv.url = "github:cachix/devenv";
  };

  nixConfig = {
    extra-trusted-public-keys = "devenv.cachix.org-1:w1cLUi8dv3hnoSPGAuibQv+f9TZLr6cv/Hm9XgU50cw=";
    extra-substituters = "https://devenv.cachix.org";
  };

  outputs = {
    self,
    nixpkgs,
    devenv,
    systems,
    ...
  } @ inputs: let
    forEachSystem = nixpkgs.lib.genAttrs (import systems);
  in {
    packages = forEachSystem (system: let
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      vertec = pkgs.stdenv.mkDerivation {
        name = "vertec";
        src = ./.;
        nativeBuildInputs = with pkgs; [cmake llvmPackages_17.llvm];
        buildInputs = with pkgs; [llvmPackages_17.libllvm zlib];
        cmakeFlags = ["-DINSTALL_VERTE=ON"];
        installPhase = ''
          cmake --install . --prefix $out
        '';
      };
    });

    defaultPackage = forEachSystem (system: self.packages.${system}.vertec);
    devShells = forEachSystem (system: let
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      verte-dev = devenv.lib.mkShell {
        inherit inputs pkgs;
        modules = [
          {
            packages = with pkgs; [
              clang-tools
              cmake
              doxygen
              zlib
              gcc
              gnumake
              gtest
              llvmPackages_17.libllvm
              valgrind
              gdb
              self.packages.${system}.vertec
            ];

            pre-commit.hooks = {
              clang-format.enable = true;
              clang-tidy.enable = true;
            };

            scripts.verte-build.exec = ''
              cmake -S . -B build
              cmake --build build
            '';

            scripts.verte-tests.exec = ''
              cmake -S . -B build -DBUILD_TESTS=true
              cmake --build build
              ./build/tests/verte-tests
            '';
          }
        ];
      };
    });
  };
}
