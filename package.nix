{ lib
, stdenv
, pkg-config
, cmake
, ninja
, magic-enum
, boost
, antlr
, jre_minimal
# for llvm lib
, llvmPackages_17
, argparse
}:
stdenv.mkDerivation {
  name = "flux";

  src = lib.sourceByRegex ./. [
    "^include.*"
    "^src.*"
    "^grammar.*"
    "CMakeLists.txt"
  ];

  nativeBuildInputs = [ cmake llvmPackages_17.clang-tools pkg-config antlr jre_minimal ninja ];
  buildInputs = [ boost llvmPackages_17.libllvm antlr.runtime.cpp magic-enum argparse ];

  cmakeFlags = [ ];

  # todo
  preConfigure = ''
    export ANTLR4_JAR_LOCATION=${antlr.jarLocation}
  '';

  shellHook = ''
    export ANTLR4_JAR_LOCATION=${antlr.jarLocation}
  '';
}
