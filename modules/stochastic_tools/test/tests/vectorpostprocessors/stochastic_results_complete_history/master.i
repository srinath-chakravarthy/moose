[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 1
  ny = 1
[]

[Variables]
  [u]
  []
[]

[Distributions]
  [uniform_left]
    type = UniformDistribution
    lower_bound = 0
    upper_bound = 0.5
  []
  [uniform_right]
    type = UniformDistribution
    lower_bound = 1
    upper_bound = 2
  []
[]

[Samplers]
  [sample]
    type = MonteCarloSampler
    num_rows = 3
    distributions = 'uniform_left uniform_right'
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[MultiApps]
  [sub]
    type = SamplerTransientMultiApp
    input_files = sub.i
    sampler = sample
  []
[]

[Transfers]
  [runner]
    type = SamplerParameterTransfer
    multi_app = sub
    sampler = sample
    parameters = 'BCs/left/value BCs/right/value'
    to_control = 'stochastic'
  []
  [data]
    type = SamplerPostprocessorTransfer
    multi_app = sub
    sampler = sample
    vector_postprocessor = storage
    postprocessor = avg
  []
[]

[VectorPostprocessors]
  [storage]
    type = StochasticResults
    parallel_type = DISTRIBUTED
    samplers = sample
    contains_complete_history = true
  []
[]

[Executioner]
  type = Transient
  num_steps = 2
[]

[Problem]
  solve = false
  kernel_coverage_check = false
[]

[Outputs]
  [out]
    type = CSV
    execute_on = FINAL
  []
[]
