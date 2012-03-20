require_relative 'base'

class TestMkmf
  class TestSignedness < TestMkmf
    def test_typeof_builtin
      bug4144 = '[ruby-dev:42731]'
      [["", "-1"], ["signed ", "-1"], ["unsigned ", "+1"]].each do |signed, expect|
        %w[short int long].each do |type|
          assert_equal(expect.to_i, mkmf {check_signedness(signed+type)}, mkmflog(bug4144))
        end
      end
    end

    def test_typeof_typedef
      [["", "-1"], ["signed ", "-1"], ["unsigned ", "+1"]].each do |signed, expect|
        %w[short int long].each do |type|
          open("confdefs.h", "w") {|f|
            f.puts "typedef #{signed}#{type} test1_t;"
          }
          $defs.clear
          assert_equal(expect.to_i, mkmf {check_signedness("test1_t", "confdefs.h")}, MKMFLOG)
          assert_include($defs, "-DSIGNEDNESS_OF_TEST1_T=#{expect}")
        end
      end
    end
  end
end
