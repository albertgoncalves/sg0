with import <nixpkgs> {};
mkShell.override { stdenv = llvmPackages_17.stdenv; } {
    buildInputs = [
        feh
        gimp
        glfw3
        libGL
        linuxPackages.perf
        mold
        mypaint
        renderdoc
        shellcheck
    ];
    APPEND_LIBRARY_PATH = lib.makeLibraryPath [
        glfw
        libGL
    ];
    shellHook = ''
        export LD_LIBRARY_PATH="$APPEND_LIBRARY_PATH:$LD_LIBRARY_PATH"
        . .shellhook
    '';
    hardeningDisable = [ "all" ];
}
