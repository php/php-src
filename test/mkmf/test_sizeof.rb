require_relative 'base'

class TestMkmf
  class TestSizeof < TestMkmf
    def setup
      super
      @sizeof_short = config_value("SIZEOF_SHORT").to_i
      @sizeof_int = config_value("SIZEOF_INT").to_i
      @sizeof_long = config_value("SIZEOF_LONG").to_i
      @sizeof_long_long = config_value("SIZEOF_LONG_LONG").to_i
      @sizeof___int64 = config_value("SIZEOF___INT64").to_i
    end

    def test_sizeof_builtin
      %w[char short int long float double void*].each do |type|
        assert_kind_of(Integer, mkmf {check_sizeof(type)}, MKMFLOG)
      end
      assert_operator(@sizeof_short, :<=, @sizeof_int)
      assert_operator(@sizeof_int, :<=, @sizeof_long)
      assert_operator(@sizeof_long, :<=, @sizeof_long_long) unless @sizeof_long_long.zero?
      assert_equal(8, @sizeof___int64) unless @sizeof___int64.zero?
    end

    def test_sizeof_struct
      open("confdefs.h", "w") {|f|
        f.puts "typedef struct {char x;} test1_t;"
      }
      assert_equal(1, mkmf {check_sizeof("test1_t", "confdefs.h")}, MKMFLOG)

      open("confdefs.h", "w") {|f|
        f.puts "typedef struct {char x, y;} test1_t;"
      }
      assert_equal(2, mkmf {check_sizeof("test1_t", "confdefs.h")}, MKMFLOG)

      open("confdefs.h", "w") {|f|
        f.puts "typedef struct {int x;} test1_t;"
      }
      assert_equal(@sizeof_int, mkmf {check_sizeof("test1_t", "confdefs.h")}, MKMFLOG)
      open("confdefs.h", "w") {|f|
        f.puts "typedef struct {int x, y;} test1_t;"
      }
      assert_equal(2 * @sizeof_int, mkmf {check_sizeof("test1_t", "confdefs.h")}, MKMFLOG)
    end
  end
end
