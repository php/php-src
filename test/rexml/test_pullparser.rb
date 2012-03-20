require "test/unit/testcase"

require 'rexml/parsers/pullparser'

class PullParserTester < Test::Unit::TestCase
  include REXML
  def test_basics
    source = '<?xml version="1.0"?>
    <!DOCTYPE blah>
    <a>foo &lt;<b attribute="value">bar</b> nooo</a>'
    parser = REXML::Parsers::PullParser.new(source)
    res = { :text=>0 }
    until parser.empty?
      results = parser.pull
      res[ :xmldecl ] = true if results.xmldecl?
      res[ :doctype ] = true if results.doctype?
      res[ :a ] = true if results.start_element? and results[0] == 'a'
      if results.start_element? and results[0] == 'b'
        res[ :b ] = true
        assert_equal 'value', results[1]['attribute']
      end
      res[ :text ] += 1 if results.text?
    end
    [ :xmldecl, :doctype, :a, :b ].each { |tag|
      assert res[tag] , "#{tag} wasn't processed"
    }
    assert_equal 4, res[ :text ]
  rescue ParseException
    puts $!
  end

  def test_bad_document
    source = "<a><b></a>"
    parser = REXML::Parsers::PullParser.new(source)
    assert_raise(ParseException, "Parsing should have failed") {
      results = parser.pull while parser.has_next?
    }
  end

  def test_entity_replacement
    source = '<!DOCTYPE foo [
    <!ENTITY la "1234">
    <!ENTITY lala "--&la;--">
    <!ENTITY lalal "&la;&la;">
    ]><a><la>&la;</la><lala>&lala;</lala></a>'
    pp = REXML::Parsers::PullParser.new( source )
    el_name = ''
    while pp.has_next?
      event = pp.pull
      case event.event_type
      when :start_element
        el_name = event[0]
      when :text
        case el_name
        when 'la'
          assert_equal('1234', event[1])
        when 'lala'
          assert_equal('--1234--', event[1])
        end
      end
    end
  end

  def test_peek_unshift
    source = "<a><b/></a>"
    pp = REXML::Parsers::PullParser.new(source)
    # FINISH ME!
  end

  def test_inspect
    xml =  '<a id="1"><b id="2">Hey</b></a>'
    parser = Parsers::PullParser.new( xml )
    while parser.has_next?
      pull_event = parser.pull
      if pull_event.start_element?
        peek = parser.peek()
        peek.inspect
      end
    end
  end

  def test_peek
    xml =  '<a id="1"><b id="2">Hey</b></a>'
    parser = Parsers::PullParser.new( xml )
    names = %w{ a b }
    while parser.has_next?
      pull_event = parser.pull
      if pull_event.start_element?
        assert_equal( :start_element, pull_event.event_type )
        assert_equal( names.shift, pull_event[0] )
        if names[0] == 'b'
          peek = parser.peek()
          assert_equal( :start_element, peek.event_type )
          assert_equal( names[0], peek[0] )
        end
      end
    end
    assert_equal( 0, names.length )
  end
end
