require 'objspace'

#
# purpose:
#  Profile memory usage of each tests.
#
# usage:
#   RUBY_TEST_ALL_PROFILE=true make test-all
#
# output:
#   ./test_all_profile
#
# collected information:
#   - ObjectSpace.memsize_of_all
#   - GC.stat
#   - /proc/self/statm (if it exists)
#

class MiniTest::Unit::TestCase
  alias orig_run run

  $test_all_profile_out = open('test_all_profile', 'w')
  $test_all_profile_gc_stat_hash = {}

  if FileTest.exist?('/proc/self/statm')
    # for Linux (only?)
    $test_all_profile_out.puts "name\tmemsize_of_all\t" +
                                 (GC.stat.keys +
                                  %w(size resident share text lib data dt)).join("\t")

    def memprofile_test_all_result_result
      "#{self.class}\##{self.__name__}\t" \
      "#{ObjectSpace.memsize_of_all}\t" \
      "#{GC.stat($test_all_profile_gc_stat_hash).values.join("\t")}\t" \
      "#{File.read('/proc/self/statm').split(/\s+/).join("\t")}"
    end
  else
    $test_all_profile_out.puts "name\tmemsize_of_alls\t" + GC.stat.keys.join("\t")
    def memprofile_test_all_result_result
      "#{self.class}\##{self.__name__}\t" \
      "#{ObjectSpace.memsize_of_all}\t" \
      "#{GC.stat($test_all_profile_gc_stat_hash).values.join("\t")}"
    end
  end

  def run runner
    result = orig_run(runner)
    $test_all_profile_out.puts memprofile_test_all_result_result
    $test_all_profile_out.flush
    result
  end
end
