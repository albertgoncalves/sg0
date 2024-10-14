with import <nixpkgs> {};
mkShell.override { stdenv = llvmPackages_18.stdenv; } {
    buildInputs = [
        feh
        gimp
        glfw3
        libGL
        linuxPackages.perf
        mold
        renderdoc
        shellcheck
    ];
    APPEND_LIBRARY_PATH = lib.makeLibraryPath [
        glfw3
        libGL
    ];
    shellHook = ''
        export LD_LIBRARY_PATH="$APPEND_LIBRARY_PATH:$LD_LIBRARY_PATH"
        . .shellhook
    '';
    hardeningDisable = [ "all" ];
}
