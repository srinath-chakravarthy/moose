# Problem II.2
#
# The thermal conductivity of an infinitely long hollow tube varies
# linearly with temperature. It is exposed on the inner
# and outer surfaces to constant temperatures.
#
# REFERENCE:
# A. Toptan, et al. (Mar.2020). Tech. rep. CASL-U-2020-1939-000, SAND2020-3887 R. DOI:10.2172/1614683.

[Mesh]
  [./geom]
    type = GeneratedMeshGenerator
    dim = 1
    elem_type = EDGE2
    xmin = 0.2
    nx = 4
  [../]
[]

[Variables]
  [./u]
    order = FIRST
  [../]
[]

[Problem]
  coord_type = RZ
[]

[Functions]
  [./exact]
    type = ParsedFunction
    vars = 'ri ro beta ki ko ui uo'
    vals = '0.2 1.0 1e-3 5.3 5 300 0'
    value = 'uo+(ko/beta)* ( ( 1 + beta*(ki+ko)*(ui-uo)*( log(x/ro) / log(ri/ro) )/(ko^2))^0.5 -1 )'
  [../]
[]

[Kernels]
  [./heat]
    type = HeatConduction
    variable = u
  [../]
[]

[BCs]
  [./ui]
    type = DirichletBC
    boundary = left
    variable = u
    value = 300
  [../]
  [./uo]
    type = DirichletBC
    boundary = right
    variable = u
    value = 0
  [../]
[]

[Materials]
  [./property]
    type = GenericConstantMaterial
    prop_names = 'density specific_heat'
    prop_values = '1.0 1.0'
  [../]
  [./thermal_conductivity]
    type = ParsedMaterial
    f_name = 'thermal_conductivity'
    args = u
    function = '5 + 1e-3 * (u-0)'
  [../]
[]

[Executioner]
  type = Steady
[]

[Postprocessors]
  [./error]
    type = ElementL2Error
    function = exact
    variable = u
  [../]
  [./h]
    type = AverageElementSize
  []
[]

[Outputs]
  csv = true
[]
