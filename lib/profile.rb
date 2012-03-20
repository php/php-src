require 'profiler'

RubyVM::InstructionSequence.compile_option = {
  :trace_instruction => true,
  :specialized_instruction => false
}
END {
  Profiler__::print_profile(STDERR)
}
Profiler__::start_profile
