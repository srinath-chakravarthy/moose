## Prerequisites

Install the following using apt-get

```bash
sudo apt-get install build-essential \
  gfortran \
  tcl \
  git \
  m4 \
  freeglut3 \
  doxygen \
  libblas-dev \
  liblapack-dev \
  libx11-dev \
  libnuma-dev \
  libcurl4-gnutls-dev \
  zlib1g-dev \
  libhwloc-dev \
  libxml2-dev \
  libpng-dev \
  pkg-config \
  liblzma-dev
```

Download and install one of our redistributable packages according to your version of Ubuntu.

- Ubuntu 18.04: [!package!name arch=ubuntu18]
- Ubuntu 16.04: [!package!name arch=ubuntu16]

Once downloaded, the package can be installed via the dpkg utility:

```bash
sudo dpkg -i moose-environment_ubuntu-*.deb
```
