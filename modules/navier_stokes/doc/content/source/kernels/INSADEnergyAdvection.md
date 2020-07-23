# INSADEnergyAdvection

This object adds a $\rho c_p \vec u \cdot \nabla T$ term, likely to a heat
conduction-convection equation, where $\rho$ is the density, $c_p$ is the
specific heat capacity,
$\vec u$ is the velocity (represented by `_U` in the code), and $\nabla T$ is the temperature gradient
(represented by `_grad_u` in the code).

!syntax description /Kernels/INSADEnergyAdvection

!syntax parameters /Kernels/INSADEnergyAdvection

!syntax inputs /Kernels/INSADEnergyAdvection

!syntax children /Kernels/INSADEnergyAdvection
