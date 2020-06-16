## Install Miniconda3 id=installconda

Installing Miniconda3 is straight forward. Download, install, and configure. If you run into issues during these steps, please visit our [troubleshooting guide for Conda](troubleshooting.md#condaissues).

- +Linux Users:+

  ```bash
  curl -L -O https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
  bash Miniconda3-latest-Linux-x86_64.sh -b -p ~/miniconda3
  ```

- +Macintosh Users:+

  ```bash
  curl -L -O https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-x86_64.sh
  bash Miniconda3-latest-MacOSX-x86_64.sh -b -p ~/miniconda3
  ```

With Miniconda installed to your home directory, export PATH, so that it may be used:

```bash
export PATH=$HOME/miniconda3/bin:$PATH
```

Configure Conda to work with conda-forge, and our mooseframework.org channel:

```bash
conda config --add channels conda-forge
conda config --add channels https://mooseframework.org/conda/moose
```
