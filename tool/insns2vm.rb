#!ruby

require 'optparse'

Version = %w$Revision: 11626 $[1..-1]

require "#{File.join(File.dirname(__FILE__), 'instruction')}"

if $0 == __FILE__
  opts = ARGV.options
  maker = RubyVM::SourceCodeGenerator.def_options(opts)
  files = opts.parse!
  generator = maker.call
  generator.generate(files)
end
