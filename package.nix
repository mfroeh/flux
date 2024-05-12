{ lib
, stdenv
, pkg-config
, cmake
, ninja
, clang-tools
, magic-enum
, boost
, antlr
, jre_minimal
, libllvm
, libffi
, argparse
}:
stdenv.mkDerivation {
  name = "flux";

  src = lib.sourceByRegex ./. [
    "^src.*"
    "^grammar.*"
    "CMakeLists.txt"
  ];

  nativeBuildInputs = [ cmake clang-tools pkg-config antlr jre_minimal ninja ];
  buildInputs = [ boost libllvm libffi antlr.runtime.cpp magic-enum argparse ];

  cmakeFlags = [ ];

  # todo
  preConfigure = ''
    export ANTLR4_JAR_LOCATION=${antlr.jarLocation}
  '';

  shellHook = ''
    export ANTLR4_JAR_LOCATION=${antlr.jarLocation}
  '';
}
