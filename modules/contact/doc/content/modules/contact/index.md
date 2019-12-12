# Contact Module

The interaction of moving bodies is a common occurrence in our world, and therefore modeling such problems is essential to accurately represent the mechanical behavior of the physical world. However, finite element methods do not have an inherent means of modeling contact. Therefore, specific contact algorithms are required. These algorithms enforce constraints between surfaces in the mesh, to prevent penetration and develop contact forces. The MOOSE contact module provides the necessary tools for modeling mechanical contact.

[](---)

## Theory

Mechanical contact between two deformable bodies is based on three requirements.

\begin{equation*}
g \le 0,
\end{equation*}
\begin{equation*}
t_N \ge 0,
\end{equation*}
\begin{equation*}
t_N g = 0.
\end{equation*}


That is, the penetration distance (typically referred to as the gap $g$ in the contact literature) of one of the body into another must not be positive; the contact force $t_N$ opposing penetration must be positive in the normal direction; and either the penetration distance or the contact force must be zero at all times.

In the MOOSE Contact Module, these contact constraints are enforced through the use of node/face constraints. This is accomplished in a manner similar to that detailed by [!cite](heinstein_algorithm_1999). First, a geometric search determines which slave nodes have penetrated master faces. For those nodes, the internal force computed by the divergence of stress is moved to the appropriate master face at the point of contact. Those forces are distributed to master nodes by employing the finite element shape functions. Additionally, the slave nodes are constrained to remain on the master faces, preventing penetration. The module currently supports frictionless, frictional, and glued contact.

[](---)

## Procedure for using mechanical contact

In the contact module there are currently two systems to choose from mechanical contact : Dirac and Constraint.  The Constraint system is recommended for all problems, as the Dirac system will be removed in the future.
The contact block in the MOOSE input file looks like this :

```puppet
[Contact]
  [./contact]
    disp_x = <variable>
    disp_y = <variable>
    disp_z = <variable>
    formulation = <string> (DEFAULT)
    friction_coefficient = <real> (0)
    master = <string>
    model = <string> (frictionless)
    normal_smoothing_distance = <real>
    normal_smoothing_method = <string> (edge_based)
    order = <string> (FIRST)
    penalty = <real> (1e8)
    normalize_penalty = <bool> (false)
    slave = <string>
    system = <string> (Dirac)
    tangential_tolerance = <real>
    tension_release = <real> (0)
  [../]
[]
```

The parameters descriptions are :

- `disp_x` (+Required+) Variable name for displacement variable in x direction. Typically `disp_x`.
- `disp_y` Variable name for displacement variable in y direction. Typically `disp_y`.
- `disp_z` Variable name for displacement variable in z direction. Typically `disp_z`
- `formulation` Select either `DEFAULT`, `KINEMATIC`, or `PENALTY`. `DEFAULT` is `KINEMATIC`.
- `friction_coefficient` The friction coefficient.
- `master` (+Required+) The boundary ID for the master surface.
- `model` Select either `frictionless`, `glued`, or `coulomb`.
- `normal_smoothing_distance` Distance from face edge in parametric coordinates over which to smooth the contact normal. $0.1$ is a reasonable value.
- `normal_smoothing_method` Select either `edge_based` or `nodal_normal_based`. If `nodal_normal_based`, must also have a `NodalNormals` block.
- `order` The order of the variable. Typical values are `FIRST` and `SECOND`.
- `penalty` The penalty stiffness value to be used in the constraint.
- `normalize_penalty` Whether to normalize the penalty stiffness by the nodal area of the slave node.
- `slave` (+Required+) The boundary ID for the slave surface.
- `system` The system to use for constraint enforcement. Options are Dirac `DiracKernel` or `Constraint`. The default is `Dirac`.
- `tangential_tolerance` Tangential distance to extend edges of contact surfaces.
- `tension_release` Tension release threshold. A node will not be released if its tensile load is below this value. If negative, no tension release will occur.


It is good practice to make the surface with the coarser mesh to be the master surface.

The robustness and accuracy of the mechanical contact algorithm is strongly dependent on the penalty parameter. If the parameter is too small, inaccurate solutions are more likely. If the parameter is too large, the solver may struggle.

The `DEFAULT` option uses an enforcement algorithm that moves the internal forces at a slave node to Thu master face. The distance between the slave node and the master face is penalized, The `PENALTY` algorithm is the traditional penalty enforcement technique.

[](---)

## Mortar-Based Mechanical Contact

The [mortar constraint system](Constraints/index.md) provides an alternative
discretization technique for solving mechanical contact. Some results are summarized below.

 The `contact/test/tests/bouncing-block-contact` directory provides a series of
input files testing different algorithms for solving both normal and tangential
frictional contact. Mortar contact can also be specified using the `Contact` block
similar to pure node-face discretization contact. For normal mortar contact:

!listing test/tests/mechanical-small-problem/frictionless-nodal-lm-mortar-disp-action.i block=Contact

For normal and tangential (frictional) mortar contact:

!listing test/tests/bouncing-block-contact/frictional-nodal-min-normal-lm-mortar-fb-tangential-lm-mortar-action.i block=Contact


### Frictionless contact algorithm comparison id=frictionless_table

| Lagrange multiplier | Displacement | NCP function | Time (arbitrary units) | Time steps | Nonlinear iterations |
| ------------------- | ------------ | ------------ | ---------------------- | ---------- | -------------------- |
| Nodal | Mortar | Min | 14.401 | 40 | 104 |
| Nodal | Mortar | FB | 17.752 | 40 | 135 |
| Nodal | Nodal | Min | 5.438 | 41 | 104 |
| Nodal | Nodal | FB | 6.770 | 41 | 149 |
| Mortar | Mortar | Min | 14.454 | 40 | 106 |
| Mortar | Mortar | FB | 19.027 | 40 | 136 |

The first column denotes the discretization algorithm used for applying the
frictionless contact constraints. Nodal denotes use of a `NodeFaceConstraint`;
`Mortar` denotes use of a `MortarConstraint`. The second column denotes the
discretization used for applying the contact forces to the displacement
residuals. The third column denotes the type of non-linear complimentarity
problem (NCP) function used to ensure that the contact constraints are
satisfied. Min indicates the canonical min function (see
[std::min](https://en.cppreference.com/w/cpp/algorithm/min)); FB represents the
Fischer-Burmeister function. The fourth column is the simulation time in
arbitrary units (since timings will be different across machines). The fifth
column is the number of time steps required to reach the simulation end
time. The final, sixth column is the cumulative number of non-linear iterations
taken during the simulation (note that this does not include any non-linear
iterations from failed time steps).

Notes:

- Clearly having mortar mesh generation slows the simulation down, which is not surprising
- The min NCP function is undeniably better for solving normal contact
- For the pure nodal algorithms, the time step that did not converge featured classic ping-ponging behavior:

```
 5 Nonlinear |R| = 4.007951e-04
    |residual|_2 of individual variables:
                  disp_x:    0.000399808
                  disp_y:    2.75599e-05
                  normal_lm: 5.52166e-06


The number of nodes in contact is 11

      0 Linear |R| = 4.007951e-04
      1 Linear |R| = 1.287307e-04
      2 Linear |R| = 8.423398e-06
      3 Linear |R| = 1.046825e-07
      4 Linear |R| = 8.017310e-09
      5 Linear |R| = 3.053040e-10
  Linear solve converged due to CONVERGED_RTOL iterations 5
 6 Nonlinear |R| = 4.432193e-04
    |residual|_2 of individual variables:
                  disp_x:    0.000396694
                  disp_y:    0.00019545
                  normal_lm: 2.96013e-05


The number of nodes in contact is 11

      0 Linear |R| = 4.432193e-04
      1 Linear |R| = 1.355935e-04
      2 Linear |R| = 1.216010e-05
      3 Linear |R| = 6.386952e-07
      4 Linear |R| = 2.235594e-08
      5 Linear |R| = 2.884193e-10
  Linear solve converged due to CONVERGED_RTOL iterations 5
 7 Nonlinear |R| = 4.008045e-04
    |residual|_2 of individual variables:
                  disp_x:    0.000399816
                  disp_y:    2.76329e-05
                  normal_lm: 5.29313e-06


The number of nodes in contact is 11

      0 Linear |R| = 4.008045e-04
      1 Linear |R| = 1.287272e-04
      2 Linear |R| = 8.423081e-06
      3 Linear |R| = 1.047782e-07
      4 Linear |R| = 8.054781e-09
      5 Linear |R| = 3.046073e-10
  Linear solve converged due to CONVERGED_RTOL iterations 5
 8 Nonlinear |R| = 4.432194e-04
```

### Frictional contact algorithm comparison id=frictional_table

| LM normal | LM tangential | Displacement | NCP function normal | NCP function tangential | Time (arbitrary units) | Time steps | Nonlinear iterations | CLI PETSc options |
| --------- | ------------  | ------------ | ------------------- | ----------------------- | ---------------------- | ---------- | -------------------- | --- |
| Mortar | Mortar | Mortar | FB | FB | 8.241 | 40 | 175 | None |
| Mortar | Mortar | Mortar | Min | FB | 7.928 | 40 | 159 | None |
| Nodal | Mortar | Mortar | Min | FB | 7.459 | 40 | 152 | None |
| Mortar | Mortar | Mortar | Min | Min | 11.237 | 41 | 234 | None |
| Nodal | Nodal | Mortar | Min | Min | 39.409 | 55 | 275 | `-snes_ksp_ew 0` |
| Nodal | Nodal | Mortar | FB | FB | NA | NA | NA | None |

Notes:

- NA: solve did not converge
- Timings run on a different machine than the frictionless cases
- The most performant case uses a `NodeFaceConstraint` discretization for
  enforcing the normal contact conditions and `MortarConstraint` discretizations
  for enforcement of the Coulomb frictional constraints and application of
  forces to the displacement residuals. Interestingly, this performant case uses
  different NCP functions for normal and tangential constraints: `std::min` for
  the former and Fischer-Burmeister for the latter. This performant case is used
  for comparison with the node-face penalty algorithm, shown below:

### NCP-LM-Mortar vs Penalty-NodeFace

The table below compares the timing and solver performance of
[NCP-LM-Mortar](/frictional_lm.i) and
[Penalty-NodeFace](/frictional_04_penalty.i) algorithms. NCP-LM refers to use of an
NCP function for contact constraint enforcement on a lagrange multiplier. The
"Mortar" designation denotes that a mortar discretization is used for enforcing
the tangential Coulomb friction conditions and applying contact forces to the
displacement residuals.

| Algorithm | Time (arb. units) | Time steps to end time | Cumulative non-linear iterations |
| --- | --- | --- | --- |
| NCP-LM-Mortar | 13.901 | 151 | 476 |
| Penalty-NodeFace | 20.711 | 151 | 938 |

There's a cost associated with generation of the mortar segment mesh that
partially offsets the fact that the mortar case takes nearly half the non-linear
iterations of the penalty case.

## Petsc options for contact

Recommended PETSc options for use with mortar based frictional contact are:

```puppet
[Executioner]
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_ksp_ew'
  petsc_options_iname = '-pc_type -mat_mffd_err -pc_factor_shift_type -pc_factor_shift_amount'
  petsc_options_value = 'lu       1e-5          NONZERO               1e-15'
[]
```

Using Eisenstat-Walker is advantageous for frictional contact because time is
not wasted in the linear solve in early non-linear iterations while the contact
set and stick/slip conditions are being resolved. Later in the non-linear solve
when the set of constraints has been resolved, more linear iterations will be
used as the non-linear solver moves through the quadratic basin. Experience has
shown that a choice of 1e-5 for the matrix free finite differencing parameter
works well for many problems. However, the user may want to experiment with
values anywhere between 1e-8 and 1e-4 depending on their multi-physics. A
very small non-zero shift is used to avoid zero pivots during the LU
decomposition. This may be extraneous in many cases. Note that the Jacobian
entries for mortar based contact are accurate and complete enough that
incomplete factorization may be used in serial or as a sub-block solver for
block jacobi or additive schwarz in parallel. This may be necessary for large
problems where lu does not scale.

The recommended PETSc options for use with `NodeFaceConstraint` based contact are shown below :

```puppet
[Executioner]
  ...
  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap
                        -ksp_gmres_restart'
  petsc_options_value = 'asm lu 20 101'
  ...
[../]
```

## Objects, Actions, and Syntax

!syntax complete groups=ContactApp level=3
