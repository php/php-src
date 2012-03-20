require 'test/unit'
require_relative 'envutil.rb'

class TestCase < Test::Unit::TestCase
  def test_case
    case 5
    when 1, 2, 3, 4, 6, 7, 8
      assert(false)
    when 5
      assert(true)
    end

    case 5
    when 5
      assert(true)
    when 1..10
      assert(false)
    end

    case 5
    when 1..10
      assert(true)
    else
      assert(false)
    end

    case 5
    when 5
      assert(true)
    else
      assert(false)
    end

    case "foobar"
    when /^f.*r$/
      assert(true)
    else
      assert(false)
    end

    case
    when true
      assert(true)
    when false, nil
      assert(false)
    else
      assert(false)
    end

    case "+"
    when *%w/. +/
      assert(true)
    else
      assert(false)
    end

    case
    when *[], false
      assert(false)
    else
      assert(true)
    end

    case
    when *false, []
      assert(true)
    else
      assert(false)
    end

    assert_raise(NameError) do
      case
      when false, *x, false
      end
    end
  end

  def test_deoptimization
    assert_in_out_err(['-e', <<-EOS], '', %w[42], [])
      class Symbol; undef ===; def ===(o); p 42; true; end; end; case :foo; when :foo; end
    EOS

    assert_in_out_err(['-e', <<-EOS], '', %w[42], [])
      class Fixnum; undef ===; def ===(o); p 42; true; end; end; case 1; when 1; end
    EOS
  end

  def test_optimization
    case 1
    when 0.9, 1.1
      assert(false)
    when 1.0
      assert(true)
    else
      assert(false)
    end
    case 536870912
    when 536870911.9, 536870912.1
      assert(false)
    when 536870912.0
      assert(true)
    else
      assert(false)
    end
  end
end
