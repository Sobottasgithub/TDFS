{
  description = "Tablo Distributed File Server";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";

    tablog = {
      url = "github:Sobottasgithub/tablog";
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      tablog,
    }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };

      version = "0.0.1";

      libtablog = tablog.packages.${system}.lib;

      commonDeps = with pkgs; [
        cmake
        gcc
        gnumake
        libtablog
      ];

      mkTdfsPackage =
        {
          pname,
          buildTarget,
          enableTdfs ? false,
          enableCli ? false,
          extraInputs ? [ ],
        }:
        pkgs.stdenv.mkDerivation {
          inherit pname version;
          src = ./.;

          buildInputs = commonDeps ++ extraInputs;

          configurePhase = ''
            cmake -B build -S $src \
              -DCMAKE_BUILD_TYPE=Release \
              -DDEF_TDFS=${if enableTdfs then "ON" else "OFF"} \
              -DDEF_CLI=${if enableCli then "ON" else "OFF"}
          '';

          buildPhase = ''
            cmake --build build \
              --target ${buildTarget} \
              -j$NIX_BUILD_CORES
          '';

          installPhase = ''
            cmake --install build --prefix=$out
            cp LICENSE $out/
          '';
        };

    in
    {
      packages.${system} =
        let
          inherit libtablog;

          tdfs = mkTdfsPackage {
            pname = "tdfs";
            buildTarget = "tdfs";
            enableTdfs = true;
            extraInputs = [ ];
          };

          cli = mkTdfsPackage {
            pname = "tdfs-cli";
            buildTarget = "tdfs-cli";
            enableCli = true;
            extraInputs = [ ];
          };
        in
        {
          inherit tdfs;
          tdfs-cli = cli;

          default = tdfs;
        };

      devShells.${system}.default = pkgs.mkShell {
        packages = commonDeps ++ [
          pkgs.bridge-utils
          pkgs.clang-tools

          pkgs.man-db
          pkgs.man-pages
          pkgs.man-pages-posix
          pkgs.stdman
        ];

        extraOutputsToInstall = [
          "man"
          "doc"
        ];

        shellHook = ''
          export MANPATH="${pkgs.man-pages}/share/man:${pkgs.man-pages-posix}/share/man:$MANPATH"
          git status
        '';
      };
    };
}
