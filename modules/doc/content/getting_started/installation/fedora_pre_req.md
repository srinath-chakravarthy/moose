## Prerequisites

Install the following using dnf

```bash
sudo -E dnf install gcc \
  gcc-c++ \
  gcc-gfortran \
  tcl \
  tk \
  findutils \
  make \
  freeglut-devel \
  libXt-devel \
  libX11-devel \
  m4 \
  blas-devel \
  lapack-devel \
  git \
  xz-devel
```

Download and install one our redistributable packages according to your version of Fedora.

- Fedora 31: [!package!name arch=fedora31]

Once downloaded, the package can be installed via the rpm utility:

```bash
sudo rpm -i moose-environment_fedora-*.rpm
```
