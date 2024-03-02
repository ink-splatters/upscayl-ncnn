with import <nixpkgs> { };
mkShellNoCC {
  buildInputs = [ vulkan-headers vulkan-loader ];

  nativeBuildInputs = [ cmake ninja glslang ];
}

