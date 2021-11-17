# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.205.2/containers/rust/.devcontainer/base.Dockerfile

# [Choice] Debian OS version (use bullseye on local arm64/Apple Silicon): buster, bullseye
ARG VARIANT="bullseye"
FROM mcr.microsoft.com/vscode/devcontainers/rust:0-${VARIANT}

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends build-essential gdb-multiarch libusb-1.0 openocd

RUN rustup component add clippy llvm-tools-preview rustfmt
RUN cargo install cargo-binutils probe-run
RUN chmod -R ug+rwx /usr/local/cargo
