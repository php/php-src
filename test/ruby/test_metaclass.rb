require 'test/unit'

class TestMetaclass < Test::Unit::TestCase
  class Foo; end
  class Bar < Foo; end
  class Baz; end

  def setup
    Object.class_eval do
      def method_o; end
    end
    Module.class_eval do
      def method_m; end
    end
    Class.class_eval do
      def method_c; end
    end
  end
  def teardown
    Object.class_eval do
      remove_method :method_o rescue nil
    end
    Module.class_eval do
      remove_method :method_m rescue nil
    end
    Class.class_eval do
      remove_method :method_c rescue nil
    end
    Object.class_eval do
      class << self
        remove_method :class_method_o rescue nil
      end
    end
    Module.class_eval do
      class << self
        remove_method :class_method_m rescue nil
      end
    end
    Class.class_eval do
      class << self
        remove_method :class_method_c rescue nil
      end
    end
    Object.class_eval do
      class << self
        class << self
          remove_method :metaclass_method_o rescue nil
        end
      end
    end
    Module.class_eval do
      class << self
        class << self
          remove_method :metaclass_method_m rescue nil
        end
      end
    end
    Class.class_eval do
      class << self
        class << self
          remove_method :metaclass_method_c rescue nil
        end
      end
    end
  end

  def test_metaclass
    class << Object
      def class_method_o; end
    end
    class << Foo
      def class_method_f; end
    end
    class << Baz
      def class_method_b; end
    end
    assert_nothing_raised{ Bar.method_o }
    assert_nothing_raised{ Bar.method_m }
    assert_nothing_raised{ Bar.method_c }
    assert_nothing_raised{ Bar.class_method_o }
    assert_nothing_raised{ Bar.class_method_f }
    assert_raise(NoMethodError){ Bar.class_method_b }

    class << Module
      def class_method_m; end
    end
    class << Class
      def class_method_c; end
    end
    class << Object
      class << self
        def metaclass_method_o; end
      end
    end
    class << Foo
      class << self
        def metaclass_method_f; end
      end
    end
    class << Baz
      class << self
        def metaclass_method_b; end
      end
    end
    metaclass_of_bar = class << Bar; self end
    assert_nothing_raised{ metaclass_of_bar.method_o }
    assert_nothing_raised{ metaclass_of_bar.method_m }
    assert_nothing_raised{ metaclass_of_bar.method_c }
    assert_nothing_raised{ metaclass_of_bar.class_method_o }
    assert_raise(NoMethodError){ metaclass_of_bar.class_method_f }
    assert_raise(NoMethodError){ metaclass_of_bar.class_method_b }
    assert_nothing_raised{ metaclass_of_bar.class_method_m }
    assert_nothing_raised{ metaclass_of_bar.class_method_c }
    assert_nothing_raised{ metaclass_of_bar.metaclass_method_o }
    assert_nothing_raised{ metaclass_of_bar.metaclass_method_f }
    assert_raise(NoMethodError){ metaclass_of_bar.metaclass_method_b }

    class << Module
      class << self
        def metaclass_method_m; end
      end
    end
    class << Class
      class << self
        def metaclass_method_c; end
      end
    end
    class << Object
      class << self
        class << self
          def metametaclass_method_o; end
        end
      end
    end
    class << Foo
      class << self
        class << self
          def metametaclass_method_f; end
        end
      end
    end
    class << Baz
      class << self
        class << self
          def metametaclass_method_b; end
        end
      end
    end
    metametaclass_of_bar = class << metaclass_of_bar; self end
    assert_nothing_raised{ metametaclass_of_bar.method_o }
    assert_nothing_raised{ metametaclass_of_bar.method_m }
    assert_nothing_raised{ metametaclass_of_bar.method_c }
    assert_nothing_raised{ metametaclass_of_bar.class_method_o }
    assert_raise(NoMethodError){ metametaclass_of_bar.class_method_f }
    assert_raise(NoMethodError){ metametaclass_of_bar.class_method_b }
    assert_nothing_raised{ metametaclass_of_bar.class_method_m }
    assert_nothing_raised{ metametaclass_of_bar.class_method_c }
    assert_nothing_raised{ metametaclass_of_bar.metaclass_method_o }
    assert_raise(NoMethodError){ metametaclass_of_bar.metaclass_method_f }
    assert_raise(NoMethodError){ metametaclass_of_bar.metaclass_method_b }
    assert_nothing_raised{ metametaclass_of_bar.metaclass_method_m }
    assert_nothing_raised{ metametaclass_of_bar.metaclass_method_c }
    assert_nothing_raised{ metametaclass_of_bar.metametaclass_method_o }
    assert_nothing_raised{ metametaclass_of_bar.metametaclass_method_f }
    assert_raise(NoMethodError){ metametaclass_of_bar.metaclass_method_b }
  end
end
