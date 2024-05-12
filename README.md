# Flux
Developing
1. Enter devshell: `nix develop`
2. Build compiler: `mkdir build && cd build && cmake .. && make && ./flux`
3. Make work with clangd: `cd .. && ln -s build/compile_commands.json .`

Packaging
1. Build package: `nix build`

Running:
1. Build and run: `nix run` or `nix build && ./result/bin/flux`
