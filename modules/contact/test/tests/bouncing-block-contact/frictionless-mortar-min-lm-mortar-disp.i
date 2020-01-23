starting_point = 2e-1
offset = 1e-2

[GlobalParams]
  displacements = 'disp_x disp_y'
  diffusivity = 1e0
  scaling = 1e0
[]

[Mesh]
  file = long-bottom-block-1elem-blocks.e
[]

[Variables]
  [./disp_x]
    block = '1 2'
  [../]
  [./disp_y]
    block = '1 2'
  [../]
  [./normal_lm]
    block = 3
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[ICs]
  [./disp_y]
    block = 2
    variable = disp_y
    value = ${fparse starting_point + offset}
    type = ConstantIC
  [../]
[]

[Kernels]
  [./disp_x]
    type = MatDiffusion
    variable = disp_x
  [../]
  [./disp_y]
    type = MatDiffusion
    variable = disp_y
  [../]
[]


[Constraints]
  [normal_lm]
    type = NormalMortarLMMechanicalContact
    master_boundary = 20
    slave_boundary = 10
    master_subdomain = 4
    slave_subdomain = 3
    variable = normal_lm
    slave_variable = disp_x
    slave_disp_y = disp_y
    use_displaced_mesh = true
    compute_primal_residuals = false
    ncp_function_type = 'min'
  []
  [normal_x]
    type = NormalMortarMechanicalContact
    master_boundary = 20
    slave_boundary = 10
    master_subdomain = 4
    slave_subdomain = 3
    variable = normal_lm
    slave_variable = disp_x
    component = x
    use_displaced_mesh = true
    compute_lm_residuals = false
  []
  [normal_y]
    type = NormalMortarMechanicalContact
    master_boundary = 20
    slave_boundary = 10
    master_subdomain = 4
    slave_subdomain = 3
    variable = normal_lm
    slave_variable = disp_y
    component = y
    use_displaced_mesh = true
    compute_lm_residuals = false
  []
[]

[BCs]
  [./botx]
    type = DirichletBC
    variable = disp_x
    preset = false
    boundary = 40
    value = 0.0
  [../]
  [./boty]
    type = DirichletBC
    variable = disp_y
    preset = false
    boundary = 40
    value = 0.0
  [../]
  [./topy]
    type = FunctionDirichletBC
    variable = disp_y
    preset = false
    boundary = 30
    function = '${starting_point} * cos(2 * pi / 40 * t) + ${offset}'
  [../]
  [./leftx]
    type = FunctionDirichletBC
    variable = disp_x
    preset = false
    boundary = 50
    function = '1e-2 * t'
  [../]
[]

[Executioner]
  type = Transient
  end_time = 200
  dt = 5
  dtmin = .1
  solve_type = 'PJFNK'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -pc_svd_monitor -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -mat_mffd_err'
  petsc_options_value = 'lu       NONZERO               1e-15                   1e-5'
  l_max_its = 30
  nl_max_its = 20
  line_search = 'none'
[]

[Debug]
  show_var_residual_norms = true
[]

[Outputs]
  exodus = true
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Postprocessors]
  active = 'num_nl cumulative contact'
  [./num_nl]
    type = NumNonlinearIterations
  [../]
  [./cumulative]
    type = CumulativeValuePostprocessor
    postprocessor = num_nl
  [../]
  [contact]
    type = ContactDOFSetSize
    variable = normal_lm
    subdomain = '3'
    execute_on = 'nonlinear timestep_end'
  []
[]
