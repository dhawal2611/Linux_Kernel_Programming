# Use a base image
FROM ubuntu:22.04
#FROM ubuntu:latest

# Set environment variables if needed
#ENV DEBIAN_FRONTEND=noninteractive

# Install required dependencies
RUN apt-get update && \
    apt-get install -y wget && \
    apt-get install -y git \ 
    build-essential \
    coreutils \
    xsltproc \
    cmake \
    docbook-xsl \
    vim \
    net-tools \
    make \
    kmod \
    linux-headers-$(uname -r)

# Command to keep the container running or enter a shell
CMD ["bash"]



