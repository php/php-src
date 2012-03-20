require "rexml_test_utils"
require 'rexml/sax2listener'
require 'rexml/parsers/sax2parser'
require 'rexml/document'

class SAX2Tester < Test::Unit::TestCase
  include REXMLTestUtils
  include REXML
  def test_characters
    d = Document.new( "<A>@blah@</A>" )
    txt = d.root.text
    p = Parsers::SAX2Parser.new "<A>@blah@</A>"
    p.listen(:characters) {|x| assert_equal txt, x}
    p.listen(:characters, ["A"]) {|x| assert_equal txt,x}
    p.parse
  end

  def test_entity_replacement
    source = '<!DOCTYPE foo [
    <!ENTITY la "1234">
    <!ENTITY lala "--&la;--">
    <!ENTITY lalal "&la;&la;">
    ]><a><la>&la;</la><lala>&lala;</lala></a>'
    sax = Parsers::SAX2Parser.new( source )
    results = []
    sax.listen(:characters) {|x| results << x }
    sax.parse
    assert_equal 2, results.size
    assert_equal '1234', results[0]
    assert_equal '--1234--', results[1]
  end

  def test_sax2
    f = File.new(fixture_path("documentation.xml"))
    parser = Parsers::SAX2Parser.new( f )
    # Listen to all events on the following elements
    count = 0
    blok = proc { |uri,localname,qname,attributes|
      assert %w{ bugs todo }.include?(localname),
      "Mismatched name; we got '#{qname}'\nArgs were:\n\tURI: #{uri}\n\tLOCALNAME: #{localname}\n\tQNAME: #{qname}\n\tATTRIBUTES: #{attributes.inspect}\n\tSELF=#{blok}"
      count += 1
    }

    start_document = 0
    end_document = 0
    parser.listen( :start_document ) { start_document += 1 }
    parser.listen( :end_document ) { end_document += 1 }
    parser.listen( :start_element, %w{ changelog bugs todo }, &blok )
    # Listen to all events on the following elements.  Synonymous with
    # listen( :start_element, %w{ ... } )
    parser.listen( %w{ changelog bugs todo }, &blok )
    # Listen for all start element events
    parser.listen( :start_element ) { |uri,localname,qname,attributes|
    }
    listener = MySAX2Listener.new
    # Listen for all events
    parser.listen( listener )
    # Listen for all events on the given elements.  Does not include children
    # events.  Regular expressions work as well!
    parser.listen( %w{ /change/ bugs todo }, listener )
    # Test the deafening method
    blok = proc  { |uri,localname,qname,attributes|
      assert_fail "This listener should have been deafened!"
    }
    parser.listen( %w{ changelog }, &blok )
    parser.deafen( &blok )

    tc = 0
    parser.listen( :characters, %w{version} ) {|text|
      assert(text=~/@ANT_VERSION@/, "version was '#{text}'")
      tc += 1
    }

    begin
      parser.parse
    rescue => exception
      if exception.kind_of? Test::Unit::AssertionFailedError
        raise exception
      end
      puts $!
      puts exception.backtrace
    end
    assert_equal 2, count
    assert_equal 1, tc
    assert_equal 1, start_document
    assert_equal 1, end_document
  end



  # used by test_simple_doctype_listener
  # submitted by Jeff Barczewski
  class SimpleDoctypeListener
    include REXML::SAX2Listener
    attr_reader :name, :pub_sys, :long_name, :uri

    def initialize
      @name = @pub_sys = @long_name = @uri = nil
    end

    def doctype(name, pub_sys, long_name, uri)
      @name = name
      @pub_sys = pub_sys
      @long_name = long_name
      @uri = uri
    end
  end

  # test simple non-entity doctype in sax listener
  # submitted by Jeff Barczewski
  def test_simple_doctype_listener
    xml = <<-END
      <?xml version="1.0"?>
      <!DOCTYPE greeting PUBLIC "Hello Greeting DTD" "http://foo/hello.dtd">
      <greeting>Hello, world!</greeting>
    END
    parser = Parsers::SAX2Parser.new(xml)
    dtl = SimpleDoctypeListener.new
    parser.listen(dtl)
    tname = nil
    tpub_sys = nil
    tlong_name = nil
    turi = nil
    parser.listen(:doctype) do |name, pub_sys, long_name, uri|
      tname = name
      tpub_sys = pub_sys
      tlong_name = long_name
      turi = uri
    end
    parser.parse
    assert_equal 'greeting', tname, 'simple doctype block listener failed - incorrect name'
    assert_equal 'PUBLIC', tpub_sys, 'simple doctype block listener failed - incorrect pub_sys'
    assert_equal 'Hello Greeting DTD', tlong_name, 'simple doctype block listener failed - incorrect long_name'
    assert_equal 'http://foo/hello.dtd', turi, 'simple doctype block listener failed - incorrect uri'
    assert_equal 'greeting', dtl.name, 'simple doctype listener failed - incorrect name'
    assert_equal 'PUBLIC', dtl.pub_sys, 'simple doctype listener failed - incorrect pub_sys'
    assert_equal 'Hello Greeting DTD', dtl.long_name, 'simple doctype listener failed - incorrect long_name'
    assert_equal 'http://foo/hello.dtd', dtl.uri, 'simple doctype listener failed - incorrect uri'
  end

  # test doctype with missing name, should throw ParseException
  # submitted by Jeff Barczewseki
  def test_doctype_with_mising_name_throws_exception
    xml = <<-END
      <?xml version="1.0"?>
      <!DOCTYPE >
      <greeting>Hello, world!</greeting>
    END
    parser = Parsers::SAX2Parser.new(xml)
    assert_raise(REXML::ParseException, 'doctype missing name did not throw ParseException') do
      parser.parse
    end
  end


  class KouListener
    include REXML::SAX2Listener
    attr_accessor :sdoc, :edoc
    attr_reader :selem, :decl, :pi
    def initialize
      @sdoc = @edoc = @selem = false
      @decl = 0
      @pi = 0
    end
    def start_document
      @sdoc = true
    end
    def end_document
      @edoc = true
    end
    def xmldecl( *arg )
      @decl += 1
    end
    def processing_instruction( *arg )
      @pi += 1
    end
    def start_element( *arg )
      @selem = true
    end
  end

  # Submitted by Kou
  def test_begin_end_document
    parser = Parsers::SAX2Parser.new("<a/>")

    kl = KouListener.new
    parser.listen(kl)
    sd = false
    ed = false
    parser.listen(:start_document) { sd = true }
    parser.listen(:end_document) { ed = true }

    parser.parse
    assert( sd, ':start_document block failed' )
    assert( ed, ':end_document block failed' )
    assert( kl.sdoc, ':start_document listener failed' )
    assert( kl.edoc, ':end_document listener failed' )
  end

  # Submitted by Kou
  def test_listen_before_start
    # FIXME: the following comment should be a test for validity. (The xml declaration
    # is invalid).
    #parser =  Parsers::SAX2Parser.new( "<?xml ?><?pi?><a><?pi?></a>")
    parser =  Parsers::SAX2Parser.new( "<?xml version='1.0'?><?pi?><a><?pi?></a>")
    k1 = KouListener.new
    parser.listen( k1 )
    xmldecl = false
    pi = 0
    parser.listen( :xmldecl ) { xmldecl = true }
    parser.listen( :processing_instruction ) { pi += 1 }

    parser.parse

    assert( xmldecl, ':xmldecl failed' )
    assert_equal( 2, pi, ':processing_instruction failed' )
    assert( k1.decl, 'Listener for xmldecl failed' )
    assert_equal( 2, k1.pi, 'Listener for processing instruction failed' )
  end


  def test_socket
    require 'socket'

    server = TCPServer.new('127.0.0.1', 0)
    socket = TCPSocket.new('127.0.0.1', server.addr[1])

    ok = false
    session = server.accept
    session << '<foo>'
    parser = REXML::Parsers::SAX2Parser.new(socket)
    Fiber.new do
      parser.listen(:start_element) do
        ok = true
        Fiber.yield
      end
      parser.parse
    end.resume
    assert(ok)
  end

  def test_char_ref_sax2()
    parser = REXML::Parsers::SAX2Parser.new('<ABC>&#252;</ABC>')
    result = nil
    parser.listen(:characters) {|text| result = text.unpack('U*')}
    parser.parse()
    assert_equal(1, result.size)
    assert_equal(252, result[0])
  end


  def test_char_ref_dom()
    doc = REXML::Document.new('<ABC>&#252;</ABC>')
    result = doc.root.text.unpack('U*')
    assert_equal(1, result.size)
    assert_equal(252, result[0])
  end

  class Ticket68
    include REXML::SAX2Listener
  end
  def test_ticket_68
    parser = REXML::Parsers::SAX2Parser.new(File.new(fixture_path('ticket_68.xml')))
    parser.listen( Ticket68.new )
    begin
      parser.parse
    rescue
      p parser.source.position
      p parser.source.current_line
      puts $!.backtrace.join("\n")
      flunk $!.message
    end
  end
end

class MySAX2Listener
  include REXML::SAX2Listener
end

