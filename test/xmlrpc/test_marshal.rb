require 'test/unit'
require "xmlrpc/marshal"

class Test_Marshal < Test::Unit::TestCase
  # for test_parser_values
  class Person
    include XMLRPC::Marshallable
    attr_reader :name
    def initialize(name)
      @name = name
    end
  end


  def test1_dump_response
    assert_nothing_raised(NameError) {
      XMLRPC::Marshal.dump_response('arg')
    }
  end

  def test1_dump_call
    assert_nothing_raised(NameError) {
      XMLRPC::Marshal.dump_call('methodName', 'arg')
    }
  end

  def test2_dump_load_response
    value = [1, 2, 3, {"test" => true}, 3.4]
    res   = XMLRPC::Marshal.dump_response(value)

    assert_equal(value, XMLRPC::Marshal.load_response(res))
  end

  def test2_dump_load_call
    methodName = "testMethod"
    value      = [1, 2, 3, {"test" => true}, 3.4]
    exp        = [methodName, [value, value]]

    res   = XMLRPC::Marshal.dump_call(methodName, value, value)

    assert_equal(exp, XMLRPC::Marshal.load_call(res))
  end

  def test_parser_values
    v1 = [
      1, -7778, -(2**31), 2**31-1,     # integers
      1.0, 0.0, -333.0, 2343434343.0,  # floats
      false, true, true, false,        # booleans
      "Hallo", "with < and >", ""      # strings
    ]

    v2 = [
      [v1, v1, v1],
      {"a" => v1}
    ]

    v3 = [
      XMLRPC::Base64.new("\001"*1000), # base64
      :aSymbol, :anotherSym            # symbols (-> string)
    ]
    v3_exp = [
      "\001"*1000,
      "aSymbol", "anotherSym"
    ]
    person = Person.new("Michael")

    XMLRPC::XMLParser.each_installed_parser do |parser|
      m = XMLRPC::Marshal.new(parser)

      assert_equal( v1, m.load_response(m.dump_response(v1)) )
      assert_equal( v2, m.load_response(m.dump_response(v2)) )
      assert_equal( v3_exp, m.load_response(m.dump_response(v3)) )

      pers = m.load_response(m.dump_response(person))

      assert( pers.is_a?(Person) )
      assert( person.name == pers.name )
    end

    # missing, Date, Time, DateTime
    # Struct
  end

  def test_parser_invalid_values
    values = [
      -1-(2**31), 2**31,
      Float::INFINITY, -Float::INFINITY, Float::NAN
    ]
    XMLRPC::XMLParser.each_installed_parser do |parser|
      m = XMLRPC::Marshal.new(parser)

      values.each do |v|
        assert_raise(RuntimeError, "#{v} shouldn't be dumped, but dumped") \
          { m.dump_response(v) }
      end
    end
  end

  def test_no_params_tag
    # bug found by Idan Sofer

    expect = %{<?xml version="1.0" ?><methodCall><methodName>myMethod</methodName><params/></methodCall>\n}

    str = XMLRPC::Marshal.dump_call("myMethod")
    assert_equal(expect, str)
  end

end
