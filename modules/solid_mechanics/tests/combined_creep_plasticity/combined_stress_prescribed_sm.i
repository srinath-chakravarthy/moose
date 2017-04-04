#
# 1x1x1 unit cube with time-varying pressure on top face
#
# The problem is a one-dimensional creep analysis.  The top face has a
#    pressure load that is a function of time.  The creep strain can be
#    calculated analytically.  There is no practical active linear
#    isotropic plasticity because the yield stress for the plasticity
#    model is set to 1e30 MPa, which will not be reached in this
#    regression test.
#
# The analytic solution to this problem is:
#
#    d ec
#    ---- = a*S^b  with S = c*t^d
#     dt
#
#    d ec = a*c^b*t^(b*d) dt
#
#         a*c^b
#    ec = ----- t^(b*d+1)
#         b*d+1
#
#    where S  = stress
#          ec = creep strain
#          t  = time
#          a  = constant
#          b  = constant
#          c  = constant
#          d  = constant
#
# With a = 3e-24,
#      b = 4,
#      c = 1,
#      d = 1/2, and
#      t = 32400
#   we have
#
#   S = t^(1/2) = 180
#
#   ec = 1e-24*t^3 = 3.4012224e-11
#
[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 1
  displacements = 'disp_x disp_y disp_z'
[]

[Variables]
  [./disp_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_z]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[AuxVariables]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./creep_strain_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Functions]
  [./pressure]
    type = ParsedFunction
    value = 'sqrt(t)'
  [../]
[]

[SolidMechanics]
  [./solid]
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
  [../]
[]

[AuxKernels]
  [./stress_yy]
    type = MaterialTensorAux
    variable = stress_yy
    tensor = stress
    index = 1
  [../]

  [./creep_strain_yy]
    type = MaterialTensorAux
    variable = creep_strain_yy
    tensor = creep_strain
    index = 1
  [../]
[]


[BCs]
  [./top_pressure]
    type = Pressure
    variable = disp_y
    component = 1
    boundary = top
    function = pressure
  [../]
  [./u_bottom_fix]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  [../]
  [./u_yz_fix]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]
  [./u_xy_fix]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0.0
  [../]
[]

[Materials]
  [./creep_plas]
    type = PLC_LSH
    block = 0
    youngs_modulus = 2.8e7
    poissons_ratio = .3
    coefficient = 3.0e-24
    n_exponent = 4
    m_exponent = 0
    activation_energy = 0
    relative_tolerance = 1.e-5
    max_its = 100
    hardening_constant = 1
    yield_stress = 1e30
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    output_iteration_info = false
  [../]
[]

[Executioner]
  type = Transient

  #Preconditioned JFNK (default)
  solve_type = 'PJFNK'


  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -sub_pc_type'
  petsc_options_value = '101           asm      lu'


  line_search = 'none'


  l_max_its = 100
  nl_max_its = 100
  nl_rel_tol = 1e-5
  nl_abs_tol = 1e-5
  l_tol = 1e-5
  start_time = 0.0
  end_time = 32400
  dt = 1e-2
  [./TimeStepper]
    type = FunctionDT
    time_dt = '1e-2 1e-1 1e0 1e1 1e2'
    time_t  = '0    7e-1 7e0 7e1 1e2'
  [../]
[]

[Postprocessors]
  [./timestep]
    type = TimestepSize
  [../]
[]

[Outputs]
  file_base = combined_stress_prescribed_out
  exodus = true
[]