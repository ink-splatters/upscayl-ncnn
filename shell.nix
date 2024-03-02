with import <nixpkgs> { };
mkShell.override { inherit (llvmPackages_17) stdenv; } {
  buildInputs = with darwin.apple_sdk.frameworks;
    with llvmPackages_17; [
      openmp
      vulkan-headers
      vulkan-loader
      libcxx
      libcxxabi
      Metal
      QuartzCore
      CoreGraphics
      Cocoa
      IOKit
      IOSurface
      Foundation
    ];
  nativeBuildInputs = [ cmake ninja lld_17 glslang llvmPackages_17.bintools ];
}
