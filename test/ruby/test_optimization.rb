require 'test/unit'

class TestRubyOptimization < Test::Unit::TestCase

  BIGNUM_POS_MIN_32 = 1073741824      # 2 ** 30
  if BIGNUM_POS_MIN_32.kind_of?(Fixnum)
    FIXNUM_MAX = 4611686018427387903  # 2 ** 62 - 1
  else
    FIXNUM_MAX = 1073741823           # 2 ** 30 - 1
  end

  BIGNUM_NEG_MAX_32 = -1073741825     # -2 ** 30 - 1
  if BIGNUM_NEG_MAX_32.kind_of?(Fixnum)
    FIXNUM_MIN = -4611686018427387904 # -2 ** 62
  else
    FIXNUM_MIN = -1073741824          # -2 ** 30
  end

  def redefine_method(klass, method)
    (@redefine_method_seq ||= 0)
    seq = (@redefine_method_seq += 1)
    eval(<<-End, ::TOPLEVEL_BINDING)
      class #{klass}
        alias redefine_method_orig_#{seq} #{method}
        undef #{method}
        def #{method}(*args)
          args[0]
        end
      end
    End
    begin
      return yield
    ensure
      eval(<<-End, ::TOPLEVEL_BINDING)
        class #{klass}
          undef #{method}
          alias #{method} redefine_method_orig_#{seq}
        end
      End
    end
  end

  def test_fixnum_plus
    a, b = 1, 2
    assert_equal 3, a + b
    assert_instance_of Fixnum, FIXNUM_MAX
    assert_instance_of Bignum, FIXNUM_MAX + 1

    assert_equal 21, 10 + 11
    assert_equal 11, redefine_method('Fixnum', '+') { 10 + 11 }
    assert_equal 21, 10 + 11
  end

  def test_fixnum_minus
    assert_equal 5, 8 - 3
    assert_instance_of Fixnum, FIXNUM_MIN
    assert_instance_of Bignum, FIXNUM_MIN - 1

    assert_equal 5, 8 - 3
    assert_equal 3, redefine_method('Fixnum', '-') { 8 - 3 }
    assert_equal 5, 8 - 3
  end

  def test_fixnum_mul
    assert_equal 15, 3 * 5
  end

  def test_fixnum_div
    assert_equal 3, 15 / 5
  end

  def test_fixnum_mod
    assert_equal 1, 8 % 7
  end

  def test_float_plus
    assert_equal 4.0, 2.0 + 2.0
    assert_equal 2.0, redefine_method('Float', '+') { 2.0 + 2.0 }
    assert_equal 4.0, 2.0 + 2.0
  end

  def test_string_length
    assert_equal 6, "string".length
    assert_nil redefine_method('String', 'length') { "string".length }
    assert_equal 6, "string".length
  end

  def test_string_plus
    assert_equal "", "" + ""
    assert_equal "x", "x" + ""
    assert_equal "x", "" + "x"
    assert_equal "ab", "a" + "b"
    assert_equal 'b', redefine_method('String', '+') { "a" + "b" }
    assert_equal "ab", "a" + "b"
  end

  def test_string_succ
    assert_equal 'b', 'a'.succ
    assert_equal 'B', 'A'.succ
  end

  def test_string_format
    assert_equal '2', '%d' % 2
  end

  def test_array_plus
    assert_equal [1,2], [1]+[2]
  end

  def test_array_minus
    assert_equal [2], [1,2] - [1]
  end

  def test_array_length
    assert_equal 0, [].length
    assert_equal 3, [1,2,3].length
  end

  def test_hash_length
    assert_equal 0, {}.length
    assert_equal 1, {1=>1}.length
  end

  class MyObj
    def ==(other)
      true
    end
  end

  def test_eq
    assert_equal true, nil == nil
    assert_equal true, 1 == 1
    assert_equal true, 'string' == 'string'
    assert_equal true, 1 == MyObj.new
    assert_equal false, nil == MyObj.new
    assert_equal true, MyObj.new == 1
    assert_equal true, MyObj.new == nil
  end

  def test_tailcall
    bug4082 = '[ruby-core:33289]'

    option = {
      tailcall_optimization: true,
      trace_instruction: false,
    }
    iseq = RubyVM::InstructionSequence.new(<<-EOF, "Bug#4082", bug4082, nil, option).eval
      class #{self.class}::Tailcall
        def fact_helper(n, res)
          if n == 1
            res
          else
            fact_helper(n - 1, n * res)
          end
        end
        def fact(n)
          fact_helper(n, 1)
        end
      end
    EOF
    assert_equal(9131, Tailcall.new.fact(3000).to_s.size, bug4082)
  end
end
