{
  description = "A statically typed, imperative programming language";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { flake-parts, ... }@inputs: flake-parts.lib.mkFlake { inherit inputs; } {
    systems = [ "x86_64-linux" "i686-linux" "aarch64-linux" "x86_64-darwin" ];
    perSystem = { config, self', inputs', pkgs, system, ... }: {
      packages.default = pkgs.callPackage ./package.nix { stdenv = pkgs.clangStdenv; };

      devShells.default = config.packages.default;
    };
  };
}
