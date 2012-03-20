require 'test/unit'
require "-test-/symbol/symbol"

module Test_Symbol
  class TestInadvertent < Test::Unit::TestCase
    def noninterned_name(prefix = "")
      prefix += "_#{Thread.current.object_id.to_s(36).tr('-', '_')}"
      begin
        name = "#{prefix}_#{rand(0x1000).to_s(16)}_#{Time.now.usec}"
      end while Bug::Symbol.interned?(name)
      name
    end

    def setup
      @obj = Object.new
    end

    def assert_not_interned(name, msg = nil)
      assert_not_send([Bug::Symbol, :interned?, name], msg)
    end

    def assert_not_interned_error(obj, meth, name, msg = nil)
      e = assert_raise(NameError, msg) {obj.__send__(meth, name)}
      assert_not_interned(name, msg)
      e
    end

    def assert_not_interned_false(obj, meth, name, msg = nil)
      assert_not_send([obj, meth, name], msg)
      assert_not_interned(name, msg)
    end

    Feature5072 = '[ruby-core:38367]'

    def test_module_const_get
      cl = Class.new
      name = noninterned_name("A")

      assert_not_interned_false(cl, :const_defined?, name, Feature5072)
    end

    def test_respond_to_missing
      feature5072 = Feature5072
      c = Class.new do
        def self.respond_to_missing?(*)
          super
        end
      end
      s = noninterned_name

      # assert_not_interned_false(c, :respond_to?, s, feature5072)
      assert_not_interned_false(c, :method_defined?, s, feature5072)
      assert_not_interned_false(c, :public_method_defined?, s, feature5072)
      assert_not_interned_false(c, :private_method_defined?, s, feature5072)
      assert_not_interned_false(c, :protected_method_defined?, s, feature5072)
      assert_not_interned_false(c, :const_defined?, noninterned_name("A"), feature5072)
      assert_not_interned_false(c, :instance_variable_defined?, noninterned_name("@"), feature5072)
      assert_not_interned_false(c, :class_variable_defined?, noninterned_name("@@"), feature5072)
    end

    Feature5079 = '[ruby-core:38404]'

    def test_undefined_instance_variable
      feature5079 = feature5079
      c = Class.new
      iv = noninterned_name("@")

      assert_not_interned_false(c, :instance_variable_get, iv, feature5079)
      assert_not_interned_error(c, :remove_instance_variable, iv, feature5079)
    end

    def test_undefined_class_variable
      feature5079 = feature5079
      c = Class.new
      cv = noninterned_name("@@")

      assert_not_interned_error(c, :class_variable_get, cv, feature5079)
      assert_not_interned_error(c, :remove_class_variable, cv, feature5079)
    end


    def test_undefined_const
      feature5079 = feature5079
      c = Class.new
      s = noninterned_name("A")

      assert_not_interned_error(c, :remove_const, s, feature5079)
    end

    def test_undefined_method
      feature5079 = feature5079
      c = Class.new
      s = noninterned_name

      assert_not_interned_error(c, :method, s, feature5079)
      assert_not_interned_error(c, :public_method, s, feature5079)
      assert_not_interned_error(c, :instance_method, s, feature5079)
      assert_not_interned_error(c, :public_instance_method, s, feature5079)
    end

    Feature5089 = '[ruby-core:38447]'
    def test_const_missing
      feature5089 = Feature5089
      c = Class.new do
        def self.const_missing(const_name)
          raise NameError, const_name.to_s
        end
      end
      s = noninterned_name("A")

      # assert_not_interned_error(c, :const_get, s, feature5089)
      assert_not_interned_false(c, :autoload?, s, feature5089)
    end

    def test_aliased_method
      feature5089 = Feature5089
      c = Class.new do
        def self.alias_method(str)
          super(:puts, str)
        end
      end
      s = noninterned_name

      assert_not_interned_error(c, :alias_method, s, feature5089)
      assert_not_interned_error(c, :private_class_method, s, feature5089)
      assert_not_interned_error(c, :private_constant, s, feature5089)
      assert_not_interned_error(c, :private, s, feature5089)
      assert_not_interned_error(c, :protected, s, feature5089)
      assert_not_interned_error(c, :public, s, feature5089)
      assert_not_interned_error(c, :public_class_method, s, feature5089)
      assert_not_interned_error(c, :public_constant, s, feature5089)
      assert_not_interned_error(c, :remove_method, s, feature5089)
      assert_not_interned_error(c, :undef_method, s, feature5089)
      assert_not_interned_error(c, :untrace_var, s, feature5089)
    end

    Feature5112 = '[ruby-core:38576]'

    def test_public_send
      name = noninterned_name
      e = assert_raise(NoMethodError) {@obj.public_send(name, Feature5112)}
      assert_not_send([Bug::Symbol, :interned?, name])
      assert_equal(name, e.name)
      assert_equal([Feature5112], e.args)
    end

    def test_send
      name = noninterned_name
      e = assert_raise(NoMethodError) {@obj.send(name, Feature5112)}
      assert_not_send([Bug::Symbol, :interned?, name])
      assert_equal(name, e.name)
      assert_equal([Feature5112], e.args)
    end

    def test___send__
      name = noninterned_name
      e = assert_raise(NoMethodError) {@obj.__send__(name, Feature5112)}
      assert_not_send([Bug::Symbol, :interned?, name])
      assert_equal(name, e.name)
      assert_equal([Feature5112], e.args)
    end
  end
end
