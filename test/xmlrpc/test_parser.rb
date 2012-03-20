require 'test/unit'
require 'xmlrpc/datetime'
require "xmlrpc/parser"
require 'yaml'

module GenericParserTest
  def datafile(base)
    File.join(File.dirname(__FILE__), "data", base)
  end

  def load_data(name)
    [File.read(datafile(name) + ".xml"), YAML.load(File.read(datafile(name) + ".expected"))]
  end

  def setup
    @xml1, @expected1 = load_data('xml1')
    @xml2, @expected2 = load_data('bug_covert')
    @xml3, @expected3 = load_data('bug_bool')
    @xml4, @expected4 = load_data('value')

    @cdata_xml, @cdata_expected = load_data('bug_cdata')

    @datetime_xml = File.read(datafile('datetime_iso8601.xml'))
    @datetime_expected = XMLRPC::DateTime.new(2004, 11, 5, 1, 15, 23)

    @fault_doc = File.read(datafile('fault.xml'))
  end

  # test parseMethodResponse --------------------------------------------------

  def test_parseMethodResponse1
    assert_equal(@expected1, @p.parseMethodResponse(@xml1))
  end

  def test_parseMethodResponse2
    assert_equal(@expected2, @p.parseMethodResponse(@xml2))
  end

  def test_parseMethodResponse3
    assert_equal(@expected3, @p.parseMethodResponse(@xml3))
  end

  def test_cdata
    assert_equal(@cdata_expected, @p.parseMethodResponse(@cdata_xml))
  end

  def test_dateTime
    assert_equal(@datetime_expected, @p.parseMethodResponse(@datetime_xml)[1])
  end

  # test parseMethodCall ------------------------------------------------------

  def test_parseMethodCall
    assert_equal(@expected4, @p.parseMethodCall(@xml4))
  end

  # test fault ----------------------------------------------------------------

  def test_fault
    flag, fault = @p.parseMethodResponse(@fault_doc)
     assert_equal(flag, false)
     unless fault.is_a? XMLRPC::FaultException
       assert(false, "must be an instance of class XMLRPC::FaultException")
     end
     assert_equal(fault.faultCode, 4)
     assert_equal(fault.faultString, "an error message")
  end

  def test_fault_message
    fault = XMLRPC::FaultException.new(1234, 'an error message')
    assert_equal('an error message', fault.to_s)
    assert_equal('#<XMLRPC::FaultException: an error message>', fault.inspect)
  end
end

# create test class for each installed parser
XMLRPC::XMLParser.each_installed_parser do |parser|
  klass = parser.class
  name = klass.to_s.split("::").last

  eval %{
    class Test_#{name} < Test::Unit::TestCase
      include GenericParserTest

      def setup
        super
        @p = #{klass}.new
      end
    end
  }
end
