[StochasticTools]
[]

[Distributions]
  [k_dist]
    type = Uniform
    lower_bound = 1
    upper_bound = 10
  []
  [q_dist]
    type = Uniform
    lower_bound = 9000
    upper_bound = 11000
  []
  [L_dist]
    type = Uniform
    lower_bound = 0.01
    upper_bound = 0.05
  []
  [Tinf_dist]
    type = Uniform
    lower_bound = 290
    upper_bound = 310
  []
[]

[Samplers]
  [pc_sampler]
    type = Quadrature
    order = 8
    distributions = 'k_dist q_dist L_dist Tinf_dist'
    execute_on = PRE_MULTIAPP_SETUP
  []
  [pr_sampler]
    type = LatinHypercube
    distributions = 'k_dist q_dist L_dist Tinf_dist'
    num_rows = 6560
    num_bins = 10
    execute_on = PRE_MULTIAPP_SETUP
  []
[]

[MultiApps]
  [pc_sub]
    type = SamplerFullSolveMultiApp
    input_files = sub.i
    sampler = pc_sampler
  []
  [pr_sub]
    type = SamplerFullSolveMultiApp
    input_files = sub.i
    sampler = pr_sampler
  []
[]

[Controls]
  [pc_cmdline]
    type = MultiAppCommandLineControl
    multi_app = pc_sub
    sampler = pc_sampler
    param_names = 'Materials/conductivity/prop_values Kernels/source/value Mesh/xmax BCs/right/value'
  []
  [pr_cmdline]
    type = MultiAppCommandLineControl
    multi_app = pr_sub
    sampler = pr_sampler
    param_names = 'Materials/conductivity/prop_values Kernels/source/value Mesh/xmax BCs/right/value'
  []
[]

[Transfers]
  [pc_data]
    type = SamplerPostprocessorTransfer
    multi_app = pc_sub
    sampler = pc_sampler
    to_vector_postprocessor = pc_results
    from_postprocessor = 'max'
  []
  [pr_data]
    type = SamplerPostprocessorTransfer
    multi_app = pr_sub
    sampler = pr_sampler
    to_vector_postprocessor = pr_results
    from_postprocessor = 'max'
  []
[]

[VectorPostprocessors]
  [pc_results]
    type = StochasticResults
  []
  [pr_results]
    type = StochasticResults
  []
[]

[Trainers]
  [pc_max]
    type = PolynomialChaosTrainer
    execute_on = timestep_end
    order = 8
    distributions = 'k_dist q_dist L_dist Tinf_dist'
    sampler = pc_sampler
    results_vpp = pc_results
    results_vector = pc_data:max
  []
  [pr_max]
    type = PolynomialRegressionTrainer
    regression_type = "ols"
    execute_on = timestep_end
    max_degree = 4
    sampler = pr_sampler
    results_vpp = pr_results
    results_vector = pr_data:max
    []
[]

[Outputs]
  [pc_out]
    type = SurrogateTrainerOutput
    trainers = 'pc_max'
    execute_on = FINAL
  []
  [pr_out]
    type = SurrogateTrainerOutput
    trainers = 'pr_max'
    execute_on = FINAL
  []
[]
