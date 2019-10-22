[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 2
    xmax = 2
    ny = 2
    ymax = 2
    elem_type = QUAD4
  []
  [./subdomain_id]
    input = gen
    type = SubdomainBoundingBoxGenerator
    bottom_left = '1 0 0'
    top_right = '2 2 0'
    block_id = 1
  [../]
  [./split]
    type = BreakMeshByBlockGenerator
    input = subdomain_id
  [../]
[]



[Variables]
  [./u]
    block = 0
  [../]
  [./v]
    block = 1
  [../]
[]


[Kernels]
  [./diff]
    type = MatDiffusion
    variable = u
    diffusivity = 'diffusivity'
    block = 0
  [../]

  [./diff_v]
    type = MatDiffusion
    variable = v
    diffusivity = 'diffusivity'
    block = 1
  [../]
[]

[InterfaceKernels]
  [tied]
    type = PenaltyInterfaceDiffusion
    variable = u
    neighbor_var = v
    penalty = 1e6
    jump_prop_name = "average_jump"
    boundary = 'interface'
  []
[]

[BCs]
  [u_left]
    type = DirichletBC
    boundary = 'left'
    variable = u
    value = 1
  []
  [v_right]
    type = DirichletBC
    boundary = 'right'
    variable = v
    value = 0
  []
[]

[Materials]
  [./stateful1]
    type = StatefulMaterial
    block = 0
    initial_diffusivity = 1
    # outputs = all
  [../]
  [./stateful2]
    type = StatefulMaterial
    block = 1
    initial_diffusivity = 2
    # outputs = all
  [../]
  [./interface_material_avg]
      type = InterfaceValueMaterial
      mat_prop_master = diffusivity
      mat_prop_slave = diffusivity
      var_master = diffusivity_var
      var_slave = diffusivity_var
      mat_prop_out_basename = diff
      boundary = interface
      interface_value_type = average
      mat_prop_var_out_basename = diff_var
      nl_var_master = u
      nl_var_slave = v
      couple_old_values_and_properties = true
  [../]
  [./interface_material_jump_master_minus_slave]
      type = InterfaceValueMaterial
      mat_prop_master = diffusivity
      mat_prop_slave = diffusivity
      var_master = diffusivity_var
      var_slave = diffusivity_var
      mat_prop_out_basename = diff
      boundary = interface
      interface_value_type = jump_master_minus_slave
      mat_prop_var_out_basename = diff_var
      nl_var_master = u
      nl_var_slave = v
      couple_old_values_and_properties = true
  [../]
  [./interface_material_jump_slave_minus_master]
      type = InterfaceValueMaterial
      mat_prop_master = diffusivity
      mat_prop_slave = diffusivity
      var_master = diffusivity_var
      var_slave = diffusivity_var
      mat_prop_out_basename = diff
      boundary = interface
      interface_value_type = jump_slave_minus_master
      mat_prop_var_out_basename = diff_var
      nl_var_master = u
      nl_var_slave = v
      couple_old_values_and_properties = true
  [../]
  [./interface_material_jump_abs]
      type = InterfaceValueMaterial
      mat_prop_master = diffusivity
      mat_prop_slave = diffusivity
      var_master = diffusivity_var
      var_slave = diffusivity_var
      mat_prop_out_basename = diff
      boundary = interface
      interface_value_type = jump_abs
      mat_prop_var_out_basename = diff_var
      nl_var_master = u
      nl_var_slave = v
      couple_old_values_and_properties = true
  [../]
  [./interface_material_master]
      type = InterfaceValueMaterial
      mat_prop_master = diffusivity
      mat_prop_slave = diffusivity
      var_master = diffusivity_var
      var_slave = diffusivity_var
      mat_prop_out_basename = diff
      boundary = interface
      interface_value_type = master
      mat_prop_var_out_basename = diff_var
      nl_var_master = u
      nl_var_slave = v
      couple_old_values_and_properties = true
  [../]
  [./interface_material_slave]
      type = InterfaceValueMaterial
      mat_prop_master = diffusivity
      mat_prop_slave = diffusivity
      var_master = diffusivity_var
      var_slave = diffusivity_var
      mat_prop_out_basename = diff
      mat_prop_var_out_basename = diff_var
      boundary = interface
      interface_value_type = slave
      nl_var_master = u
      nl_var_slave = v
      couple_old_values_and_properties = true
  [../]
[]

[AuxKernels]
  [./interface_material_avg]
    type = MaterialRealAux
    property = diff_average
    variable = diffusivity_average
    boundary = interface
  []
  [./interface_material_jump_master_minus_slave]
    type = MaterialRealAux
    property = diff_jump_master_minus_slave
    variable = diffusivity_jump_master_minus_slave
    boundary = interface
  []
  [./interface_material_jump_slave_minus_master]
    type = MaterialRealAux
    property = diff_jump_slave_minus_master
    variable = diffusivity_jump_slave_minus_master
    boundary = interface
  []
  [./interface_material_jump_abs]
    type = MaterialRealAux
    property = diff_jump_abs
    variable = diffusivity_jump_abs
    boundary = interface
  []
  [./interface_material_master]
    type = MaterialRealAux
    property = diff_master
    variable = diffusivity_master
    boundary = interface
  []
  [./interface_material_slave]
    type = MaterialRealAux
    property = diff_slave
    variable = diffusivity_slave
    boundary = interface
  []
  [./interface_material_avg_prev]
    type = MaterialRealAux
    property = diff_average_prev
    variable = diffusivity_average_prev
    boundary = interface
  []
  [./interface_material_jump_master_minus_slave_prev]
    type = MaterialRealAux
    property = diff_jump_master_minus_slave_prev
    variable = diffusivity_jump_master_minus_slave_prev
    boundary = interface
  []
  [./interface_material_jump_slave_minus_master_prev]
    type = MaterialRealAux
    property = diff_jump_slave_minus_master_prev
    variable = diffusivity_jump_slave_minus_master_prev
    boundary = interface
  []
  [./interface_material_jump_abs_prev]
    type = MaterialRealAux
    property = diff_jump_abs_prev
    variable = diffusivity_jump_abs_prev
    boundary = interface
  []
  [./interface_material_master_prev]
    type = MaterialRealAux
    property = diff_master_prev
    variable = diffusivity_master_prev
    boundary = interface
  []
  [./interface_material_slave_prev]
    type = MaterialRealAux
    property = diff_slave_prev
    variable = diffusivity_slave_prev
    boundary = interface
  []
  [diffusivity_var]
    type = MaterialRealAux
    property = diffusivity
    variable = diffusivity_var
  []
[]

[AuxVariables]
  [diffusivity_var]
    family = MONOMIAL
    order = CONSTANT
  []
  [./diffusivity_average]
    family = MONOMIAL
    order = CONSTANT
  []
  [./diffusivity_jump_master_minus_slave]
    family = MONOMIAL
    order = CONSTANT
  []
  [./diffusivity_jump_slave_minus_master]
    family = MONOMIAL
    order = CONSTANT
  []
  [./diffusivity_jump_abs]
    family = MONOMIAL
    order = CONSTANT
  []
  [./diffusivity_master]
    family = MONOMIAL
    order = CONSTANT
  []
  [./diffusivity_slave]
    family = MONOMIAL
    order = CONSTANT
  []
  [./diffusivity_average_prev]
    family = MONOMIAL
    order = CONSTANT
  []
  [./diffusivity_jump_master_minus_slave_prev]
    family = MONOMIAL
    order = CONSTANT
  []
  [./diffusivity_jump_slave_minus_master_prev]
    family = MONOMIAL
    order = CONSTANT
  []
  [./diffusivity_jump_abs_prev]
    family = MONOMIAL
    order = CONSTANT
  []
  [./diffusivity_master_prev]
    family = MONOMIAL
    order = CONSTANT
  []
  [./diffusivity_slave_prev]
    family = MONOMIAL
    order = CONSTANT
  []
[]


[Executioner]
  type = Transient
  solve_type = NEWTON
  num_steps = 3
  dt = 0.5
[]

[Outputs]
  exodus = true
[]
