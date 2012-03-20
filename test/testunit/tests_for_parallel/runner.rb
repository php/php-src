require 'rbconfig'
require 'test/unit'

src_testdir = File.dirname(File.expand_path(__FILE__))

class Test::Unit::Runner
  @@testfile_prefix = "ptest"
end

exit Test::Unit::AutoRunner.run(true, src_testdir)
