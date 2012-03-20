require 'test/unit'
require "xmlrpc/create"
require "xmlrpc/parser"
require "xmlrpc/config"

class Test_Features < Test::Unit::TestCase

  def setup
    @params = [nil, {"test" => nil}, [nil, 1, nil]]
  end

  def test_nil_create
    XMLRPC::XMLWriter.each_installed_writer do |writer|
      c = XMLRPC::Create.new(writer)

      XMLRPC::Config.module_eval {remove_const(:ENABLE_NIL_CREATE)}
      XMLRPC::Config.const_set(:ENABLE_NIL_CREATE, false)
      assert_raise(RuntimeError) { str = c.methodCall("test", *@params) }

      XMLRPC::Config.module_eval {remove_const(:ENABLE_NIL_CREATE)}
      XMLRPC::Config.const_set(:ENABLE_NIL_CREATE, true)
      assert_nothing_raised { str = c.methodCall("test", *@params) }
    end
  end

  def test_nil_parse
    XMLRPC::Config.module_eval {remove_const(:ENABLE_NIL_CREATE)}
    XMLRPC::Config.const_set(:ENABLE_NIL_CREATE, true)

    XMLRPC::XMLWriter.each_installed_writer do |writer|
      c = XMLRPC::Create.new(writer)
      str = c.methodCall("test", *@params)
      XMLRPC::XMLParser.each_installed_parser do |parser|
        para = nil

        XMLRPC::Config.module_eval {remove_const(:ENABLE_NIL_PARSER)}
        XMLRPC::Config.const_set(:ENABLE_NIL_PARSER, false)
        assert_raise(RuntimeError) { para = parser.parseMethodCall(str) }

        XMLRPC::Config.module_eval {remove_const(:ENABLE_NIL_PARSER)}
        XMLRPC::Config.const_set(:ENABLE_NIL_PARSER, true)
        assert_nothing_raised { para = parser.parseMethodCall(str) }
        assert_equal(para[1], @params)
      end
    end
  end

end
