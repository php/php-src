$file = ARGV[0]
$str  = ARGF.read.sub(/^__END__.*\z/m, '')
puts '# ' + '-' * 70
puts "# target program: "
puts '# ' + '-' * 70
puts $str
puts '# ' + '-' * 70

$parsed = RubyVM::InstructionSequence.compile_file($file)
puts "# disasm result: "
puts '# ' + '-' * 70
puts $parsed.disasm
puts '# ' + '-' * 70
