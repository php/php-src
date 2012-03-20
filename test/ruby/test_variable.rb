require 'test/unit'
require_relative 'envutil'

class TestVariable < Test::Unit::TestCase
  class Gods
    @@rule = "Uranus"
    def ruler0
      @@rule
    end

    def self.ruler1		# <= per method definition style
      @@rule
    end
    class << self			# <= multiple method definition style
      def ruler2
	@@rule
      end
    end
  end

  module Olympians
    @@rule ="Zeus"
    def ruler3
      @@rule
    end
  end

  class Titans < Gods
    @@rule = "Cronus"			# modifies @@rule in Gods
    include Olympians
    def ruler4
      @@rule
    end
  end

  def test_variable
    assert_instance_of(Fixnum, $$)

    # read-only variable
    assert_raise(NameError) do
      $$ = 5
    end
    assert_normal_exit("$*=0; $*", "[ruby-dev:36698]")

    foobar = "foobar"
    $_ = foobar
    assert_equal(foobar, $_)

    assert_equal("Cronus", Gods.new.ruler0)
    assert_equal("Cronus", Gods.ruler1)
    assert_equal("Cronus", Gods.ruler2)
    assert_equal("Cronus", Titans.ruler1)
    assert_equal("Cronus", Titans.ruler2)
    atlas = Titans.new
    assert_equal("Cronus", atlas.ruler0)
    assert_equal("Zeus", atlas.ruler3)
    assert_equal("Cronus", atlas.ruler4)
    assert_nothing_raised do
      class << Gods
        defined?(@@rule) && @@rule
      end
    end
  end

  def test_local_variables
    lvar = 1
    assert_instance_of(Symbol, local_variables[0], "[ruby-dev:34008]")
  end

  def test_local_variables2
    x = 1
    proc do |y|
      assert_equal([:x, :y], local_variables.sort)
    end.call
  end

  def test_local_variables3
    x = 1
    proc do |y|
      1.times do |z|
        assert_equal([:x, :y, :z], local_variables.sort)
      end
    end.call
  end

  def test_global_variable_0
    assert_in_out_err(["-e", "$0='t'*1000;print $0"], "", /\At+\z/, [])
  end

  def test_global_variable_poped
    assert_nothing_raised { eval("$foo; 1") }
  end

  def test_constant_poped
    assert_nothing_raised { eval("TestVariable::Gods; 1") }
  end
end
