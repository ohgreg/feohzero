# Use a lightweight Debian base image
FROM debian:bookworm-slim

# Set the working dir
WORKDIR /engine

# Install development depedencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    clang \
    valgrind \
    gdb \
    cmake \
    ninja-build \
    git \
    curl \
    wget \
    unzip \
    pkg-config \
    manpages-dev \
    libssl-dev \
    zlib1g-dev \
    libtool \
    autoconf \
    automake \
    lldb \
    python3 \
    python3-pip \
    clang-format \
    clang-tidy \
    tar \
    xz-utils \
    openssh-client && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Add GitHub's SSH fingerprint to known hosts
RUN mkdir -p /root/.ssh && \
    ssh-keyscan github.com >> /root/.ssh/known_hosts

# Use bash as the default shell
SHELL ["/bin/bash", "-c"]

# Install and activate Emscripten
RUN git clone https://github.com/emscripten-core/emsdk.git /emsdk

RUN cd /emsdk && \
    ./emsdk install latest && \
    ./emsdk activate latest

RUN chmod +x /emsdk/emsdk_env.sh
RUN /emsdk/emsdk_env.sh > /etc/profile.d/emsdk_env.sh
ENV PATH="/emsdk:/emsdk/upstream/emscripten:${PATH}"

# Copy all project files
COPY . /engine

# Expose server's port
EXPOSE 8000

# Build the project
# CMD ["make", "run"]
