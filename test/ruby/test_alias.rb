require 'test/unit'

class TestAlias < Test::Unit::TestCase
  class Alias0
    def foo
      "foo"
    end
  end

  class Alias1 < Alias0
    alias bar foo

    def foo
      "foo+#{super}"
    end
  end

  class Alias2 < Alias1
    alias baz foo
    undef foo
  end

  class Alias3 < Alias2
    def foo
      super
    end

    def bar
      super
    end

    def quux
      super
    end
  end

  def test_alias
    x = Alias2.new
    assert_equal "foo", x.bar
    assert_equal "foo+foo", x.baz
    assert_equal "foo+foo", x.baz   # test_check for cache

    x = Alias3.new
    assert_raise(NoMethodError) { x.foo }
    assert_equal "foo", x.bar
    assert_raise(NoMethodError) { x.quux }
  end

  class C
    def m
      $SAFE
    end
  end

  def test_JVN_83768862
    d = lambda {
      $SAFE = 4
      dclass = Class.new(C)
      dclass.send(:alias_method, :mm, :m)
      dclass.new
    }.call
    assert_raise(SecurityError) { d.mm }
  end

  def test_nonexistmethod
    assert_raise(NameError){
      Class.new{
        alias_method :foobarxyzzy, :barbaz
      }
    }
  end

  def test_send_alias
    x = "abc"
    class << x
      alias_method :try, :__send__
    end
    assert_equal("ABC", x.try(:upcase), '[ruby-dev:38824]')
  end

  def test_special_const_alias
    assert_raise(TypeError) do
      1.instance_eval do
        alias to_string to_s
      end
    end
  end

  def test_alias_with_zsuper_method
    c = Class.new
    c.class_eval do
      def foo
        :ok
      end
      def bar
        :ng
      end
      private :foo
    end
    d = Class.new(c)
    d.class_eval do
      public :foo
      alias bar foo
    end
    assert_equal(:ok, d.new.bar)
  end
end
