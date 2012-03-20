require "test/unit/testcase"
require "rexml/document"
require 'rexml/streamlistener'
require 'stringio'

class MyListener
  include REXML::StreamListener
end


class StreamTester < Test::Unit::TestCase
  # Submitted by Han Holl
  def test_listener
    data = %Q{<session1 user="han" password="rootWeiler" />\n<session2 user="han" password="rootWeiler" />}

    b = RequestReader.new( data )
    b = RequestReader.new( data )
  end

  def test_ticket_49
    source = StringIO.new( <<-EOL )
    <!DOCTYPE foo [
      <!ENTITY ent "replace">
    ]>
    <a>&ent;</a>
    EOL
    REXML::Document.parse_stream(source, MyListener.new)
  end

  def test_ticket_10
    source = StringIO.new( <<-EOL )
    <!DOCTYPE foo [
      <!ENTITY ent "replace">
      <!ATTLIST a
       xmlns:human CDATA #FIXED "http://www.foo.com/human">
      <!ELEMENT bar (#PCDATA)>
      <!NOTATION n1 PUBLIC "-//HM//NOTATION TEST1//EN" 'urn:x-henrikmartensson.org:test5'>
    ]>
    <a/>
    EOL
    listener = MyListener.new
    class << listener
      attr_accessor :events
      def entitydecl( content )
        @events[ :entitydecl ] = true
      end
      def attlistdecl( element_name, attributes, raw_content )
        @events[ :attlistdecl ] = true
      end
      def elementdecl( content )
        @events[ :elementdecl ] = true
      end
      def notationdecl( content )
        @events[ :notationdecl ] = true
      end
    end
    listener.events = {}

    REXML::Document.parse_stream( source, listener )

    assert( listener.events[:entitydecl] )
    assert( listener.events[:attlistdecl] )
    assert( listener.events[:elementdecl] )
    assert( listener.events[:notationdecl] )
  end
end


# For test_listener
class RequestReader
  attr_reader :doc
  def initialize(io)
    @stack = []
    @doc = nil
    catch(:fini) do
      REXML::Document.parse_stream(io, self)
      raise IOError
    end
  end
  def tag_start(name, args)
    if @doc
      @stack.push(REXML::Element.new(name, @stack.last))
    else
      @doc = REXML::Document.new("<#{name}/>")
      @stack.push(@doc.root)
    end
    args.each do |attr,val|
      @stack.last.add_attribute(attr, val)
    end
  end
  def tag_end(name, *args)
    @stack.pop
    throw(:fini) if @stack.empty?
  end
  def text(str)
    @stack.last.text = str
  end
  def comment(str)
  end
  def doctype( name, pub_sys, long_name, uri )
  end
  def doctype_end
  end
end
