[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 2
  ny = 2
  nz = 2
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./disp_z]
  [../]
[]

[Kernels]
  [./stress_x]
    type = StressDivergenceTensors
    component = 0
    variable = disp_x
  [../]
  [./stress_y]
    type = StressDivergenceTensors
    component = 1
    variable = disp_y
  [../]
  [./stress_z]
    type = StressDivergenceTensors
    component = 2
    variable = disp_z
  [../]
[]

[BCs]
  [./symmy]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
  [../]
  [./symmx]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0
  [../]
  [./symmz]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0
  [../]
  [./tdisp]
    type = DirichletBC
    variable = disp_z
    boundary = front
    value = 0.1
  [../]
[]

[Materials]
  [./strain]
    type = ADComputeSmallStrain
  [../]
[]

[Materials]
  [./elasticity]
    type = ComputeIsotropicElasticityTensor
    poissons_ratio = 0.3
    youngs_modulus = 1e10
  [../]
  [./stress]
    type = ComputeLinearElasticStress
  [../]
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.05
  solve_type = 'NEWTON'

  petsc_options_iname = -pc_hypre_type
  petsc_options_value = boomeramg

  dtmin = 0.05
  num_steps = 1
[]

[Outputs]
  exodus = true
  file_base = "linear-out"
[]
