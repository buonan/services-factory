# Use a base image with a recent Ubuntu or Debian distribution
FROM ubuntu:22.04

# Set environment variables (optional, but good practice)
ENV DEBIAN_FRONTEND=noninteractive

# Install build essentials, CMake, and Make
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    apt-utils \
    cmake \
    clangd \
    clang-format \
    make \
    g++ \
    gcc \
    git \
    valgrind \
    gdb \
    vim \
    locales \
    && rm -rf /var/lib/apt/lists/*

# Configure locales to avoid warnings
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

# Create a non-root user for security (optional but recommended)
ARG USERNAME=ubuntu
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME && \
    useradd --uid $USER_UID --gid $USER_GID -m $USERNAME && \
    apt-get update && \
    apt-get install -y sudo && \
    echo $USERNAME ALL=\(ALL\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME && \
    chmod 0440 /etc/sudoers.d/$USERNAME

# Set the working directory
WORKDIR /workspace

# Switch to the non-root user
USER $USERNAME

# You can add more tools or libraries as needed for your specific project
# For example, if you need a specific library:
# RUN sudo apt-get update && sudo apt-get install -y libboost-dev

