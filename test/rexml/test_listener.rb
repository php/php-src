# coding: binary

require 'rexml_test_utils'

require 'rexml/document'
require 'rexml/streamlistener'

class BaseTester < Test::Unit::TestCase
  include REXMLTestUtils
  def test_empty
    return unless defined? @listener
    # Empty.
    t1 = %Q{<string></string>}
    assert_equal( "", @listener.parse( t1 ),
      "Empty" )
  end

  def test_space
    return unless defined? @listener
    # Space.
    t2 = %Q{<string>    </string>}
    assert_equal( "    ", @listener.parse( t2 ),
      "Space" )
  end

  def test_whitespace
    return unless defined? @listener
    # Whitespaces.
    t3 = %Q{<string>RE\n \t \n \t XML</string>}
    assert_equal( "RE\n \t \n \t XML", @listener.parse( t3 ),
      "Whitespaces" )
  end

  def test_leading_trailing_whitespace
    return unless defined? @listener
    # Leading and trailing whitespaces.
    t4 = %Q{<string>    REXML    </string>}
    assert_equal( "    REXML    ", @listener.parse( t4 ),
      "Leading and trailing whitespaces" )
  end

  def test_entity_reference
    return unless defined? @listener
    # Entity reference.
    t5 = %Q{<string>&lt;&gt;&amp;lt;&amp;gt;</string>}
    assert_equal( "<>&lt;&gt;", @listener.parse( t5 ),
      "Entity reference" )
  end

  def test_character_reference
    return unless defined? @listener
    # Character reference.
    t6 = %Q{<string>&#xd;</string>}
    assert_equal( "\r", @listener.parse( t6 ),
      "Character reference." )
  end

  def test_cr
    return unless defined? @listener
    # CR.
    t7 = %Q{<string> \r\n \r \n </string>}
    assert_equal( " \n \n \n ".unpack("C*").inspect,
      @listener.parse( t7 ).unpack("C*").inspect, "CR" )
  end

  # The accent bug, and the code that exibits the bug, was contributed by
  # Guilhem Vellut
  class AccentListener
    def tag_start(name,attributes)
      #p name
      #p attributes
    end
    def tag_end(name)
      #p "/"+name
    end
    def xmldecl(a,b,c)
      #puts "#{a} #{b} #{c}"
    end
    def text(tx)
      #p tx
    end
  end

  def test_accents
    source = '<?xml version="1.0" encoding="ISO-8859-1"?>
<g>
<f  a="é" />
</g>'
    doc = REXML::Document.new( source )
    a = doc.elements['/g/f'].attribute('a')
                if a.value.respond_to? :force_encoding
                  a.value.force_encoding('binary')
                end
    assert_equal( 'Ã©', a.value)
    doc = REXML::Document.parse_stream(
      File::new(fixture_path("stream_accents.xml")),
      AccentListener::new
      )
  end
end

class MyREXMLListener
  include REXML::StreamListener

  def initialize
    @text = nil
  end

  def parse( stringOrReadable )
    @text = ""
    REXML::Document.parse_stream( stringOrReadable, self )
    @text
  end

  def text( text )
    @text << text
  end
end

class REXMLTester < BaseTester
  def setup
    @listener = MyREXMLListener.new
  end

  def test_character_reference_2
    t6 = %Q{<string>&#xd;</string>}
    assert_equal( t6.strip, REXML::Document.new(t6).to_s )
  end
end
