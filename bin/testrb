#!/usr/bin/env ruby
require 'test/unit'
tests = Test::Unit::AutoRunner.new(true)
tests.options.banner.sub!(/\[options\]/, '\& tests...')
unless tests.process_args(ARGV)
  abort tests.options.banner
end
files = tests.to_run
$0 = files.size == 1 ? File.basename(files[0]) : files.to_s
exit tests.run
