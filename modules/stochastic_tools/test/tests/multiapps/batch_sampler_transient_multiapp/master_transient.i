[Mesh]
  type = GeneratedMesh
  dim = 1
[]

[Problem]
  kernel_coverage_check = false
  solve = false
[]

[Distributions]
  [uniform]
    type = UniformDistribution
    lower_bound = 2
    upper_bound = 4
  []
[]

[Samplers]
  [mc]
    type = MonteCarloSampler
    num_rows = 5
    distributions = 'uniform uniform'
    execute_on = 'INITIAL TIMESTEP_BEGIN'
  []
[]

[Executioner]
  type = Transient
  num_steps = 3
[]

[MultiApps]
  [runner]
    type = SamplerTransientMultiApp
    sampler = mc
    input_files = 'sub.i'
    execute_on = 'INITIAL TIMESTEP_BEGIN'
    mode = batch-restore
  []
[]

[Transfers]
  [runner]
    type = SamplerParameterTransfer
    multi_app = runner
    sampler = mc
    parameters = 'BCs/left/value BCs/right/value'
    to_control = 'stochastic'
  []
  [data]
    type = SamplerPostprocessorTransfer
    multi_app = runner
    sampler = mc
    to_vector_postprocessor = storage
    from_postprocessor = average
  []
[]

[VectorPostprocessors]
  [storage]
    type = StochasticResults
    samplers = mc
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Outputs]
  csv = true
[]
