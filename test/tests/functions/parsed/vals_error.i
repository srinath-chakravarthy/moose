[Mesh]
  type = GeneratedMesh
  dim = 1
[]

[Variables]
  [./dummy1]
  [../]
[]

[Problem]
  kernel_coverage_check = false
[]

[Functions]
  [./left_bc]
    type = ParsedFunction
    value = dummy2
    vals = invalid
    vars = dummy2
  [../]
[]

[Executioner]
  type = Steady
[]
