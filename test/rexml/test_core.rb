# coding: binary

require "rexml_test_utils"

require "rexml/document"
require "rexml/parseexception"
require "rexml/output"
require "rexml/source"
require "rexml/formatters/pretty"
require "rexml/undefinednamespaceexception"

require "listener"

class Tester < Test::Unit::TestCase
  include REXMLTestUtils
  include REXML
  def setup
    @xsa_source = <<-EOL
      <?xml version="1.0"?>
      <?xsl stylesheet="blah.xsl"?>
      <!-- The first line tests the XMLDecl, the second tests PI.
      The next line tests DocType. This line tests comments. -->
      <!DOCTYPE xsa PUBLIC
        "-//LM Garshol//DTD XML Software Autoupdate 1.0//EN//XML"
        "http://www.garshol.priv.no/download/xsa/xsa.dtd">

      <xsa>
        <vendor id="blah">
          <name>Lars Marius Garshol</name>
          <email>larsga@garshol.priv.no</email>
          <url>http://www.stud.ifi.uio.no/~lmariusg/</url>
        </vendor>
      </xsa>
    EOL
  end

  def test_bad_markup
    [
      "<pkg='version'> foo </pkg>",
      '<0/>',
      '<a>&</a>',
      '<a>&a</a>',
#      '<a>&a;</a>', # FIXME
      '<a a="<"/>',
      '<a 3="<"/>',
      '<a a="1" a="2"/>',
      '<a><!-- -- --></a>',
      '<a><!-- ---></a>',
      '<a>&#x00;</a>',
      '<a>&#0;</a>',
      "<a a='&#0;' />",
      "<a>\f</a>",
      "<a a='\f' />",
      "<a>\000</a>",
# FIXME      '<a' + [65535].pack('U') + ' />',
      '<a>&#xfffe;</a>',
      '<a>&#65535;</a>',
# FIXME      '<a' + [0x0371].pack('U') + ' />',
# FIXME      '<a a' + [0x0371].pack('U') + '="" />',
    ].each do |src|
      assert_raise( ParseException, %Q{Parse #{src.inspect} should have failed!} ) do
        Document.new(src)
      end
    end
  end

  def test_attribute
    # Testing constructors
    #a = Attribute.new "hello", "dolly"
    #b = Attribute.new a
    #d = Document.new( "<a hello='dolly' href='blah'/>" )
    #c = d[0].attributes.get_attribute( "hello" )

    #assert_equal a, b
    #for attr in [ a, b, c]
    #  assert_equal "hello", attr.name
    #  assert_equal "dolly", attr.value
    #end

    # This because of a reported bug in attribute handling in 1.0a8
    source = '<a att="A">blah</a>'
    doc = Document.new source
    doc.elements.each do |a|
      a.attributes['att'] << 'B'
      assert_equal "AB", a.attributes['att']
      a.attributes['att'] = 'C'
      assert_equal "C", a.attributes['att']
    end

    # Bryan Murphy <murphybryanp@yahoo.com>
    text = "this is a {target[@name='test']/@value} test"
    source = <<-EOL
    <?xml version="1.0"?>
    <doc search="#{text}"/>
    EOL

    xml  = Document.new source
    value = xml.root.attributes["search"]
    assert_equal text, value.to_s

    e = Element.new "test"
    e.add_attributes({ "name1" => "test1", "name4" => "test4" })
    e.add_attributes([["name3","test3"], ["name2","test2"]])
    assert_equal "test1", e.attributes["name1"]
    assert_equal "test2", e.attributes["name2"]
    assert_equal "test3", e.attributes["name3"]
    assert_equal "test4", e.attributes["name4"]

    # ensure that the attributes come out in sorted order
    assert_equal %w(<test
      name1='test1'
      name2='test2'
      name3='test3'
      name4='test4'/>).join(' '), e.to_s
  end

  def test_cdata
    test = "The quick brown fox jumped
      & < & < \" '
    over the lazy dog."

    source = "<a><![CDATA[#{test}]]></a>"
    d = REXML::Document.new( source )

    # Test constructors
    cdata = d[0][0]
    assert_equal test, cdata.value
  end

  def test_comment
    string = "This is a new comment!"
    source = "<!--#{string}-->"
    comment = Comment.new string
    REXML::Formatters::Default.new.write( comment, out = "" )
    assert_equal(source, out)

    comment2 = Comment.new comment
    assert_equal(comment, comment2)

    assert_raise(ParseException) {
      REXML::Document.new("<d><!- foo --></d>")
    }
    assert_raise(ParseException) {
      REXML::Document.new("<d><!-- foo -></d>")
    }
  end

  def test_whitespace
    doc = Document.new "<root-element><first-element/></root-element>"
    assert_equal 1, doc.root.size
    assert_equal 1, doc.root.elements.size
    doc = Document.new "<root-element>
    <first-element/>
    </root-element>"
    assert_equal 3, doc.root.size
    assert_equal 1, doc.root.elements.size

    text = "  This is   text
    with a lot of   whitespace   "
    source = "<a>#{text}<b>#{text}</b><c>#{text}</c>#{text}</a>"

    doc = Document.new( source, {
      :respect_whitespace => %w{ a c }
    } )
    assert_equal text, doc.elements["//c"].text
    string = ""
    doc.root.each { |n| string << n.to_s if n.kind_of? Text }
    assert_equal text+text, string

    string ="   lots   of    blank
    space"
    doc.root.add_element("d").add_element("c").text = string
    doc.root.add_element("e").text = string
    assert_equal string, doc.elements["/a/d/c"].text
    assert string != doc.elements["/a/e"].text, "Text wasn't properly compressed"

    doc = Document.new source, { :respect_whitespace => :all }
    doc.root.add_element("d").text = string
    assert_equal text, doc.root.text
    nxt = ""
    doc.root.each { |n| nxt << n.to_s if n.kind_of? Text }
    assert_equal text+text, nxt
    assert_equal text, doc.root.elements["b"].text
    assert_equal text, doc.root.elements["c"].text
    assert_equal string, doc.root.elements["d"].text
  end

  # This isn't complete.  We need to check declarations and comments
  def test_doctype
    string = "something"
    correct = "<!DOCTYPE something>"
    doc = DocType.new(string)
    assert_equal(string, doc.name)
    doc.write(out="")
    assert_equal(correct, out)

    doc2 = DocType.new(doc)
    assert_equal(doc.name, doc2.name)
    assert_equal(doc.external_id, doc2.external_id)

    correct = '<!DOCTYPE xsa PUBLIC "-//LM Garshol//DTD XML Software Autoupdate 1.0//EN//XML" "http://www.garshol.priv.no/download/xsa/xsa.dtd">'

    one_line_source = '<!DOCTYPE xsa PUBLIC "-//LM Garshol//DTD XML Software Autoupdate 1.0//EN//XML" "http://www.garshol.priv.no/download/xsa/xsa.dtd"><a/>'
    doc = Document.new( one_line_source )
    doc = doc[0]
    assert(doc)
    doc.write(test="")
    assert_equal(correct, test)

    multi_line_source = '<!DOCTYPE xsa PUBLIC
    "-//LM Garshol//DTD XML Software Autoupdate 1.0//EN//XML"
    "http://www.garshol.priv.no/download/xsa/xsa.dtd">
    <a/>'
    d = Document.new( multi_line_source )
    doc = d[0]
    assert(doc)
    doc.write(test="")
    assert_equal(correct, test)

    odd_space_source = '  <!DOCTYPE
    xsa      PUBLIC                 "-//LM Garshol//DTD XML Software Autoupdate 1.0//EN//XML"
    "http://www.garshol.priv.no/download/xsa/xsa.dtd">   <a/>'
    d = Document.new( odd_space_source )
    dt = d.doctype
    dt.write(test="")
    assert_equal(correct, test)

    # OK, the BIG doctype test, numba wun
    docin = File.new(fixture_path("doctype_test.xml"))
    doc = Document.new(docin)
    doc.write(test="")
    assert_equal(31, doc.doctype.size)
  end

  def test_document
    # Testing cloning
    source = "<element/>"
    doc = Document.new source

    # Testing Root
    assert_equal doc.root.name.to_s, "element"

    # Testing String source
    source = @xsa_source
    doc = Document.new source
    assert_instance_of XMLDecl, doc.xml_decl
    assert_instance_of DocType, doc.doctype
    assert_equal doc.version, "1.0"

    source = File.new(fixture_path("dash.xml"))
    doc = Document.new source
    assert_equal "content-2", doc.elements["//content-2"].name
  end

  def test_instruction
    target = "use"
    content = "ruby"
    source = "<?#{target} #{content}?>"

    instruction = Instruction.new target, content
    instruction2 = Instruction.new instruction
    assert_equal(instruction, instruction2)
    REXML::Formatters::Default.new.write( instruction, out = "" )
    assert_equal(source, out)

    d = Document.new( source )
    instruction2 = d[0]
    assert_equal(instruction.to_s, instruction2.to_s)

    assert_raise(ParseException) {
      REXML::Document.new("<d><?foo bar></d>")
    }
  end

  def test_parent
    parent = Parent.new
    begin
      parent << "Something"
    rescue Exception
      parent << Comment.new("Some comment")
      assert parent.size == 1, "size of parent should be 1"
    else
      assert_fail "should have gotten an exception trying to add a "+ "String to a Parent"
    end

    source = "<a><one/><three/><five/></a>"
    doc = Document.new source
    three = doc.root.elements["three"]
    doc.root.insert_before( three, Element.new("two") )
    nxt = doc.root.elements["one"]
    string = ""
    while nxt
      string << nxt.name
      nxt = nxt.next_sibling
    end
    assert_equal "onetwothreefive", string


    doc.root.insert_after( three, Element.new("four") )
    string = ""
    doc.root.each { |element| string << element.name }
    assert_equal "onetwothreefourfive", string

    string = ""
    nxt = doc.root.elements["five"]
    while nxt
      string << nxt.name
      nxt = nxt.previous_sibling
    end
    assert_equal "fivefourthreetwoone", string

    doc.insert_after "//two", Element.new("two-and-half")
    string = doc.root.elements.collect {|x| x.name}.join
    assert_equal "onetwotwo-and-halfthreefourfive", string
    doc.elements["/a/five"].insert_before "../four", Element.new("three-and-half")
    string = doc.root.elements.collect {|x| x.name}.join
    assert_equal "onetwotwo-and-halfthreethree-and-halffourfive", string

    doc.elements["/a/five"].previous_sibling = Element.new("four-and-half")
    string = doc.root.elements.collect {|x| x.name}.join
    assert_equal "onetwotwo-and-halfthreethree-and-halffourfour-and-halffive", string
    doc.elements["/a/one"].next_sibling = Element.new("one-and-half")
    string = doc.root.elements.collect {|x| x.name}.join
    assert_equal "oneone-and-halftwotwo-and-halfthreethree-and-halffourfour-and-halffive", string

    doc = Document.new "<a><one/><three/></a>"
    doc.root[1,0] = Element.new "two"
    string = ""
    doc.root.each { |el| string << el.name }
    assert_equal "onetwothree", string
  end

  # The Source classes are tested extensively throughout the test suite
  def test_source
    # Testing string source
    source = @xsa_source
    doc = Document.new source
    assert_equal doc.root.name.to_s, "xsa"

    # Testing IO source
    doc = Document.new File.new(fixture_path("project.xml"))
    assert_equal doc.root.name.to_s, "Project"
  end

  def test_text
    f = REXML::Formatters::Default.new
    string = "Some text"
    text = Text.new(string)
    assert_equal(string, text.to_s)
    text2 = Text.new(text)
    assert_equal(text, text2)
    #testing substitution
    string = "0 < ( 1 & 1 )"
    correct = "0 &lt; ( 1 &amp; 1 )"
    text = Text.new(string, true)
    f.write(text,out="")
    assert_equal(correct, out)

    string = "Cats &amp; dogs"
    text = Text.new(string, false, nil, true)
    assert_equal(string, text.to_s)

    string2 = "<a>#{string}</a>"
    doc = Document.new( string2, {
      :raw => %w{ a b }
    } )
    f.write(doc,out="")
    assert_equal(string2, out)
    b = doc.root.add_element( "b" )
    b.text = string
    assert_equal(string, b.get_text.to_s)

    c = doc.root.add_element("c")
    c.text = string
    assert_equal("Cats &amp;amp; dogs", c.get_text.to_s)

    # test all
    string = "<a>&amp;<b>&lt;</b><c>&gt;<d>&quot;</d></c></a>"
    doc = Document.new(string, { :raw => :all })
    assert_equal( "&amp;", doc.elements["/a"][0].to_s )
    assert_equal( "&", doc.elements["/a"].text )
    assert_equal( "&lt;", doc.elements["/a/b"][0].to_s )
    assert_equal( "<", doc.elements["/a/b"].text )
    assert_equal( "&gt;", doc.elements["/a/c"][0].to_s )
    assert_equal( ">", doc.elements["/a/c"].text )
    assert_equal( '&quot;', doc.elements["//d"][0].to_s )
    assert_equal( '"', doc.elements["//d"].text )

    # test some other stuff
    doc = Document.new('<a><b/></a>')
    doc.root.text = 'Sean'
    assert_equal( '<a><b/>Sean</a>', doc.to_s )
    doc.root.text = 'Elliott'
    assert_equal( '<a><b/>Elliott</a>', doc.to_s )
    doc.root.add_element( 'c' )
    assert_equal( '<a><b/>Elliott<c/></a>', doc.to_s )
    doc.root.text = 'Russell'
    assert_equal( '<a><b/>Russell<c/></a>', doc.to_s )
    doc.root.text = nil
    assert_equal( '<a><b/><c/></a>', doc.to_s )
  end

  def test_text_frozen
    string = "Frozen".freeze
    text = Text.new(string)
    assert_equal(string, text.to_s)
  end

  def test_xmldecl
    source = "<?xml version='1.0'?>"
    # test args
    # test no args
    decl2 = XMLDecl.new
    assert_equal source, decl2.to_s
    # test XMLDecl
    decl2 = XMLDecl.new "1.0"
    assert_equal source, decl2.to_s
  end

  def test_xmldecl_utf_16be_encoding_name
    assert_equal("<?xml version='1.0' encoding='UTF-16'?>",
                 XMLDecl.new("1.0", "UTF-16").to_s)
  end

  def each_test( element, xpath, num_children )
    count = 0
    element.each_element( xpath ) { |child|
      count += 1
      yield child if block_given?
    }
    assert_equal num_children, count
  end

  # This is the biggest test, as the number of permutations of xpath are
  # enormous.
  def test_element_access
    # Testing each_element
    doc = Document.new File.new(fixture_path("project.xml"))

    each_test( doc, "/", 1 ) { |child|
      assert_equal doc.name, child.name
    }
    each_test(doc, ".", 1) { |child| assert_equal doc, child }
    each_test(doc.root, "..", 1) { |child| assert_equal doc, child }
    each_test(doc.root, "*", 5)
    each_test(doc, "Project/Datasets", 1) { |child|
      assert_equal "Datasets", child.name
    }
    each_test(doc, "Project/Datasets/link", 2 )
    each_test(doc.root, "/Project/Description", 1) {|child|
      assert_equal "Description", child.name
    }
    each_test(doc.root, "./Description",1 ) { |child|
      assert_equal "Description",child.name
    }
    each_test(doc.root, "../Project",1 ) { |child|
      assert_equal doc.root, child
    }
    #each_test(doc,".../link",2) {|child| assert_equal "link",child.name.to_s}

    # test get_element
    first = doc.elements[ "Project" ]
    assert_equal doc.root, first
    second = doc.elements[ "Project" ].elements[1]
    third = doc.elements[ "Project/Creator" ]
    assert_equal second, third
    fourth = doc.elements[ "Project/Datasets/link[@idref='18']" ]
    assert_equal "Test data 1", fourth.attributes["name"]

    # Testing each_predicate
    each_test( doc, "Project/Datasets/link[@idref='18']", 1 ) { |child|
      assert_equal "Test data 1", child.attributes["name"]
    }

    # testing next/previous_element
    creator = doc.elements["//Creator"]
    lm = creator.next_element
    assert_equal "LastModifier", lm.name
    assert_equal "Creator", lm.previous_element.name
  end

  def test_child
    sean = Element.new "Sean"
    rubbell = Element.new "Rubbell"
    elliott = sean.add_element "Elliott"
    sean << rubbell
    assert_equal elliott, rubbell.previous_sibling
    assert_equal rubbell, elliott.next_sibling

    russell = Element.new "Russell"
    rubbell.replace_with russell
    assert_equal elliott, russell.previous_sibling
    assert_equal russell, elliott.next_sibling

    assert_nil russell.document
    assert_equal sean, russell.root
  end

  # Most of this class is tested elsewhere.  Here are the methods which
  # aren't used in any other class
  def test_element
    sean = Element.new "Sean"
    string = "1) He's a great guy!"
    sean.text = string
    russell = Element.new "Russell"
    sean << russell

    russell.attributes["email"] = "ser@germane-software.com"
    assert_equal russell.attributes["email"], "ser@germane-software.com"
    russell.attributes["webpage"] = "http://www.germane-software.com/~ser"

    assert sean.has_text?, "element should have text"
    assert_equal sean.text, string
    assert sean.has_elements?, "element should have one element"
    string = "2) What a stud!"
    sean.add_text string
    sean.text = "3) Super programmer!"
    sean.text = nil
    assert sean.has_text?, "element should still have text"
    assert_equal sean.text, string

    russell.delete_attribute "email"
    assert_nil russell.attributes["email"]
    russell.attributes.delete "webpage"
    assert !russell.has_attributes?, "element should have no attributes"
  end

  def test_no_format
    source = "<a><b><c>blah</c><d/></b></a>"
    out = ""
    doc = Document.new( source )
    doc.write(out)
    assert_equal(source, out)
  end

  def test_namespace
    source = <<-EOF
    <x xmlns:foo="http://www.bar.com/schema">
    </x>
    EOF
    doc = Document.new(source)
    assert_equal("http://www.bar.com/schema", doc.root.namespace( "foo" ))
    source = <<-EOF
    <!-- bar namespace is "someuri" -->
    <foo:bar xmlns="default" xmlns:foo="someuri">
    <!-- a namespace is "default" -->
    <a/>
    <!-- foo:b namespace is "someuri" -->
    <foo:b>
    <!-- c namespace is "default" -->
    <c/>
    </foo:b>
    <!-- d namespace is "notdefault" -->
    <d xmlns="notdefault">
    <!-- e namespace is "notdefault" -->
    <e/>
    <f xmlns="">
    <g/>
    </f>
    </d>
    </foo:bar>
    EOF
    doc = Document.new source
    assert_equal "someuri", doc.root.namespace
    assert_equal "default", doc.root.elements[1].namespace
    assert_equal "someuri", doc.root.elements[2].namespace
    assert_equal "notdefault", doc.root.elements[ 3 ].namespace

    # Testing namespaces in attributes
    source = <<-EOF
    <a xmlns:b="uri">
    <b b:a="x" a="y"/>
    <c xmlns="foo">
    </c>
    </a>
    EOF
    doc = Document.new source
    b = doc.root.elements["b"]
    assert_equal "x", b.attributes["b:a"]
    assert_equal "y", b.attributes["a"]

    doc = Document.new
    doc.add_element "sean:blah"
    doc.root.text = "Some text"
    out = ""
    doc.write(out)
    assert_equal "<sean:blah>Some text</sean:blah>", out
  end


  def test_add_namespace
    e = Element.new 'a'
    e.add_namespace 'someuri'
    e.add_namespace 'foo', 'otheruri'
    e.add_namespace 'xmlns:bar', 'thirduri'
    assert_equal 'someuri', e.attributes['xmlns']
    assert_equal 'otheruri', e.attributes['xmlns:foo']
    assert_equal 'thirduri', e.attributes['xmlns:bar']
  end


  def test_big_documentation
    f = File.new(fixture_path("documentation.xml"))
    d = Document.new f
    assert_equal "Sean Russell", d.elements["documentation/head/author"].text.tr("\n\t", " ").squeeze(" ")
    out = ""
    d.write out
  end

  def test_tutorial
    doc = Document.new File.new(fixture_path("tutorial.xml"))
    out = ""
    doc.write out
  end

  def test_stream
    c = Listener.new
    Document.parse_stream( File.new(fixture_path("documentation.xml")), c )
    assert(c.ts, "Stream parsing apparantly didn't parse the whole file")
    assert(c.te, "Stream parsing dropped end tag for documentation")

    Document.parse_stream("<a.b> <c/> </a.b>", c)

    Document.parse_stream("<a>&lt;&gt;&amp;</a>", c)
    assert_equal('<>&', c.normalize)
  end

  def test_line
    Document.new File.new(fixture_path("bad.xml"))
    assert_fail "There should have been an error"
  rescue Exception
    # We should get here
    assert($!.line == 5, "Should have been an error on line 5, "+
      "but was reported as being on line #{$!.line}" )
  end

  def test_substitution
    val = "a'b\"c"
    el = Element.new("a")
    el.attributes["x"] = val
    REXML::Formatters::Default.new.write(el, out="")

    nel = Document.new( out)
    assert_equal( val, nel.root.attributes["x"] )
  end

  def test_exception
    source = SourceFactory.create_from "<a/>"
    p = ParseException.new( "dummy message", source )
    begin
      raise "dummy"
    rescue Exception
      p.continued_exception = $!
    end
  end

  def test_bad_content
    in_gt = '<root-el>content>content</root-el>'
    in_lt = '<root-el>content<content</root-el>'

    # This is OK
    tree_gt = Document.new in_gt
    assert_equal "content>content", tree_gt.elements[1].text
    # This isn't
    begin
      Document.new in_lt
      assert_fail "Should have gotten a parse error"
    rescue ParseException
    end
  end

  def test_iso_8859_1_output_function
    out = ""
    output = Output.new( out )
    koln_iso_8859_1 = "K\xF6ln"
    koln_utf8 = "K\xc3\xb6ln"
    source = Source.new( koln_iso_8859_1, 'iso-8859-1' )
    results = source.scan(/.*/)[0]
    koln_utf8.force_encoding('UTF-8') if koln_utf8.respond_to?(:force_encoding)
    assert_equal koln_utf8, results
    output << results
    if koln_iso_8859_1.respond_to?(:force_encoding)
      koln_iso_8859_1.force_encoding('ISO-8859-1')
    end
    assert_equal koln_iso_8859_1, out
  end

  def test_attributes_each
    doc = Document.new("<a xmlns:a='foo'><b x='1' y='2' z='3' a:x='4'/></a>")
    count = 0
    doc.root.elements[1].attributes.each {|k,v| count += 1 }
    assert_equal 4, count
  end

  def test_delete_namespace
    doc = Document.new "<a xmlns='1' xmlns:x='2'/>"
    doc.root.delete_namespace
    doc.root.delete_namespace 'x'
    assert_equal "<a/>", doc.to_s
  end

  def test_each_element_with_attribute
    doc = Document.new "<a><b id='1'/><c id='2'/><d id='1'/><e/></a>"
    arry = []
    block = proc { |e|
      assert arry.include?(e.name)
      arry.delete e.name
    }
    # Yields b, c, d
    arry = %w{b c d}
    doc.root.each_element_with_attribute( 'id', &block )
    assert_equal 0, arry.size
    # Yields b, d
    arry = %w{b d}
    doc.root.each_element_with_attribute( 'id', '1', &block )
    assert_equal 0, arry.size
    # Yields b
    arry = ['b']
    doc.root.each_element_with_attribute( 'id', '1', 1, &block )
    assert_equal 0, arry.size
    # Yields d
    arry = ['d']
    doc.root.each_element_with_attribute( 'id', '1', 0, 'd', &block )
    assert_equal 0, arry.size
  end
  def test_each_element_with_text
    doc = Document.new '<a><b>b</b><c>b</c><d>d</d><e/></a>'
    arry = []
    block = proc { |e|
      assert arry.include?(e.name)
      arry.delete e.name
    }
    # Yields b, c, d
    arry = %w{b c d}
    doc.root.each_element_with_text(&block)
    assert_equal 0, arry.size
    # Yields b, d
    arry = %w{b c}
    doc.root.each_element_with_text( 'b', &block )
    assert_equal 0, arry.size
    # Yields b
    arry = ['b']
    doc.root.each_element_with_text( 'b', 1, &block )
    assert_equal 0, arry.size
    # Yields d
    arry = ['d']
    doc.root.each_element_with_text( nil, 0, 'd', &block )
    assert_equal 0, arry.size
  end

  def test_element_parse_stream
    s = Source.new( "<a>some text</a>" )
    l = Listener.new
    class << l
      def tag_start name, attributes
        raise "Didn't find proper tag name" unless 'a'==name
      end
    end

    Document::parse_stream(s, l)
  end

  def test_deep_clone
    a = Document.new( '<?xml version="1"?><a x="y"><b>text</b>text<c><d><e>text</e></d></c></a>' )
    b = a.deep_clone
    assert_equal a.to_s, b.to_s

    a = Document.new( '<a>some &lt; text <b> more &gt; text </b> &gt; </a>' )
    b = a.deep_clone
    assert_equal a.to_s, b.to_s
    c = Document.new( b.to_s )
    assert_equal a.to_s, c.to_s
  end

  def test_whitespace_before_root
    a = <<EOL
<?xml version='1.0'?>
  <blo>
    <wak>
    </wak>
  </blo>
EOL
    d = Document.new(a)
    b = ""
    d.write( b )
    assert_equal a,b
  end

  def test_entities
    a = Document.new( '<a>&#101;&#x65;&#252;</a>' )
    assert_equal('eeü'.force_encoding("UTF-8"), a.root.text)
  end

  def test_element_decl
    element_decl = Source.new("<!DOCTYPE foo [
<!ELEMENT bar (#PCDATA)>
]>")
    doc = Document.new( element_decl )
    d = doc[0]
    assert_equal("<!ELEMENT bar (#PCDATA)>", d.to_s.split(/\n/)[1].strip)
  end

  def test_attlist_decl
    doc = Document.new <<-EOL
    <!DOCTYPE blah [
    <!ATTLIST blah
      xmlns    CDATA    "foo">
    <!ATTLIST a
      bar          CDATA "gobble"
      xmlns:one    CDATA  "two"
    >
    ]>
    <a xmlns:three='xxx' three='yyy'><one:b/><three:c/></a>
    EOL
    assert_equal 'gobble', doc.root.attributes['bar']
    assert_equal 'xxx', doc.root.elements[2].namespace
    assert_equal 'two', doc.root.elements[1].namespace
    assert_equal 'foo', doc.root.namespace

    doc = Document.new <<-EOL
    <?xml version="1.0"?>
    <!DOCTYPE schema SYSTEM "XMLSchema.dtd" [
    <!ENTITY % p ''>
    <!ENTITY % s ''>
    <!ATTLIST schema
      xmlns:svg CDATA #FIXED "http://www.w3.org/2000/svg"
      xmlns:xlink CDATA #FIXED "http://www.w3.org/1999/xlink"
      xmlns:xml CDATA #FIXED "http://www.w3.org/XML/1998/namespace"
    >]>
    <schema/>
    EOL
    prefixes = doc.root.prefixes.sort
    correct = ['svg', 'xlink', 'xml']
    assert_equal correct, prefixes
  end

  def test_attlist_write
    file=File.new(fixture_path("foo.xml"))
    doc=Document.new file
    out = ''
    doc.write(out)
  end

  def test_more_namespaces
    assert_raise( REXML::UndefinedNamespaceException,
                   %Q{Should have gotten an Undefined Namespace error} )  {
      Document.new("<r><p><n:c/></p></r>")
    }
    doc2 = Document.new("<r xmlns:n='1'><p><n:c/></p></r>")
    es = XPath.match(doc2, '//c')
    assert_equal 0, es.size
    es = XPath.match(doc2, '//n:c')
    assert_equal 1, es.size
    doc2.root.add_namespace('m', '2')
    doc2.root.add_element("m:o")
    es = XPath.match(doc2, './/o')
    assert_equal 0, es.size
    es = XPath.match(doc2, '//n:c')
    assert_equal 1, es.size
  end

  def test_ticket_51
    doc = REXML::Document.new <<-EOL
      <test xmlns='1' xmlns:x='1'>
         <a>X</a>
         <x:a>Y</x:a>

         <b xmlns='2'>
           <a>Z</a>
         </b>
      </test>
    EOL

    # The most common case.  People not caring about the namespaces much.
    assert_equal( "XY", XPath.match( doc, "/test/a/text()" ).join )
    assert_equal( "XY", XPath.match( doc, "/test/x:a/text()" ).join )
    # Surprising?  I don't think so, if you believe my definition of the "common case"
    assert_equal( "XYZ", XPath.match( doc, "//a/text()" ).join )

    # These are the uncommon cases.  Namespaces are actually important, so we define our own
    # mappings, and pass them in.
    assert_equal( "XY", XPath.match( doc, "/f:test/f:a/text()", { "f" => "1" } ).join )
    # The namespaces are defined, and override the original mappings
    assert_equal( "", XPath.match( doc, "/test/a/text()", { "f" => "1" } ).join )
    assert_equal( "", XPath.match( doc, "/x:test/x:a/text()", { "f" => "1" } ).join )
    assert_equal( "", XPath.match( doc, "//a/text()", { "f" => "1" } ).join )
  end

  def test_processing_instruction
    d = Document.new("<a><?foo bar?><?foo2 bar2?><b><?foo3 bar3?></b><?foo4 bar4?></a>")
    assert_equal 4, XPath.match(d, '//processing-instruction()' ).size
    match = XPath.match(d, "//processing-instruction('foo3')" )
    assert_equal 1, match.size
    assert_equal 'bar3', match[0].content
  end

  def test_oses_with_bad_EOLs
    Document.new("\n\n\n<?xml version='1.0'?>\n\n\n<a/>\n\n")
  end

  # Contributed (with patch to fix bug) by Kouhei
  def test_ignore_whitespace
    source = "<a> <b/> abc <![CDATA[def]]>  </a>"

    context_all = {:ignore_whitespace_nodes => :all}
    context_a = {:ignore_whitespace_nodes => %(a)}
    context_b = {:ignore_whitespace_nodes => %(b)}

    tests = [[[" abc ", "def"], context_all],
             [[" abc ", "def"], context_a],
             [[" ", " abc ", "def", "  "], context_b]]

    tests.each do |test|
      assert_equal(test[0], Document.new(source, test[1]).root.texts.collect{|x|
        x.to_s})
    end
  end

  def test_0xD_in_preface
    doc = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\x0D<opml version=\"1.0\">\x0D</opml>"
    doc = Document.new doc
  end

  def test_hyphens_in_doctype
    doc = REXML::Document.new <<-EOQ
     <?xml version="1.0"?>
     <!DOCTYPE a-b-c>
     <a-b-c>
       <a/>
     </a-b-c>
    EOQ

    assert_equal('a-b-c', doc.doctype.name)
  end

  def test_accents
    docs = [
      %Q{<?xml version="1.0" encoding="ISO-8859-1"?>
<gnuPod>
<files>
  <file id="57"  artist="Coralie Cl\357\277\275ent" />
</files>
</gnuPod>},
      '<?xml version="1.0" encoding="ISO-8859-1"?>
<gnuPod>
<files>
    <file id="71"  album="Astrakan Caf" />
</files>
</gnuPod>',
      %Q{<?xml version="1.0" encoding="ISO-8859-1"?>
<gnuPod>
<files>
    <file id="71"  album="Astrakan Caf\357\277\275eria" />
</files>
</gnuPod>},
      %Q{<?xml version="1.0" encoding="ISO-8859-1"?>
<gnuPod>
<files>
    <file id="71"  album="Astrakan Caf\357\277\275" />
</files>
</gnuPod>} ]
    docs.each_with_index { |d,i|
      begin
        REXML::Document.new(d)
      rescue
        puts "#{i} => #{docs[i]}"
        raise
      end
    }
  end

  def test_replace_text
    e = REXML::Element.new( "a" )
    e.add_text( "foo" )
    assert_equal( "<a>foo</a>", e.to_s )
    e[0].value = "bar"
    assert_equal( "<a>bar</a>", e.to_s )
    e[0].value = "<"
    assert_equal( "<a>&lt;</a>", e.to_s )
    assert_equal( "<", e[0].value )
  end


  def test_write_doctype
    ## XML Document and Declaration
    document = REXML::Document.new
    xmldecl = REXML::XMLDecl.new("1.0", "UTF-8")
    document.add(xmldecl)
    s = ""
    document.write(s)

    ## XML Doctype
    str = '<!DOCTYPE foo "bar">'
    source  = REXML::Source.new(str)
    doctype = REXML::DocType.new(source)
    document.add(doctype)
    document.write(s)

    ## Element
    element = REXML::Element.new("hoge")
    document.add(element)

    document.write(s)
  end

  def test_write_cdata
    src = "<a>A</a>"
    doc = REXML::Document.new( src )
    out = ""
    doc.write( out )
    assert_equal( src, out )

    src = "<a><![CDATA[A]]></a>"
    doc = REXML::Document.new( src )
    out = ""
    doc.write( out )
    assert_equal( src, out )
  end

  def test_namespace_attributes
    source = <<-EOL
    <a xmlns:x="1">
      <x:b x:n="foo"/>
    </a>
    EOL
    d = Document.new( source )
    assert_equal( 'foo', REXML::XPath.first(d.root, "//x:b/@x:n").value )
    assert_equal( nil, REXML::XPath.first(d.root, "//x:b/@x:n", {}))
  end

  def test_null_element_name
    a = REXML::Document.new
    assert_raise( RuntimeError ) {
      a.add_element( nil )
    }
  end

  def test_text_raw
    # From the REXML tutorial
    # (http://www.germane-software.com/software/rexml/test/data/tutorial.html)
    doc = Document.new <<-EOL
    <?xml version="1.0"?>
    <!DOCTYPE schema SYSTEM "XMLSchema.dtd" [
    <!ENTITY % s 'Sean'>
    ]>
    <a/>
    EOL
    a = doc.root

    # This makes sure that RAW text nodes don't have their entity strings
    # replaced
    t = Text.new "Sean", false, nil, true
    a.text = t
    assert_equal( "Sean", t.to_s )
    assert_equal( "Sean", t.value )

    # This makes sure that they do
    t = Text.new "Sean", false, nil, false
    a.text = t
    assert_equal( "&s;", t.to_s )
    assert_equal( "Sean", t.value )

    t = Text.new "&s;", false, nil, true
    a.text = t
    assert_equal( "&s;", t.to_s )
    assert_equal( "Sean", t.value )

    t = Text.new "&s;", false, nil, true
    a.text = t
    assert_equal( "&s;", t.to_s )
    assert_equal( "Sean", t.value )

    # Ticket #44
    t = REXML::Text.new( "&amp;", false, nil, true )
    assert_equal( "&amp;", t.to_s )

    t = REXML::Text.new("&amp;", false, false)
    assert_equal( "&amp;amp;", t.to_s )
  end

  def test_to_xpath
  doc = REXML::Document.new( %q{<tag1>
      <tag2 name="tag2"/>
      <tag2 name="tag2"/>
    </tag1>})
    names = %w{ /tag1/tag2[1] /tag1/tag2[2] }
    doc.root.elements.each_with_index {|el, i|
      assert_equal( names[i], el.xpath )
    }
  end

  def test_transitive
  doc = REXML::Document.new( "<a/>")
  s = ""
  doc.write( s, 0, true )
  end

  # This is issue #40
  def test_replace_with
    old = '<doc>old<foo/>old</doc>'
    d = REXML::Document.new(old).root
    new = REXML::Text.new('new',true,nil,true)
    child = d.children[2]
    child.replace_with(new)
    assert_equal( new, d.children[2] )
  end

  def test_repeated_writes
    a = IO.read(fixture_path("iso8859-1.xml"))
    f = REXML::Formatters::Pretty.new

    xmldoc = REXML::Document.new( a )
    a_andre = xmldoc.elements['//image'].attributes['caption']

    f.write(xmldoc,b="")

    xmldoc = REXML::Document.new(b)
    b_andre = xmldoc.elements['//image'].attributes['caption']
    assert_equal( a_andre, b_andre )

    f.write(xmldoc,c="")

    xmldoc = REXML::Document.new(c)
    c_andre = xmldoc.elements['//image'].attributes['caption']
    assert_equal( b_andre, c_andre )

    o = Output.new(d="","UTF-8")
    f.write(xmldoc,o)
    assert_not_equal( c, d )
  end

  def test_pretty_format_long_text_finite
    n = 1_000_000
    long_text = 'aaaa ' * n
    xml = "<doc>#{long_text}</doc>"
    formatter = REXML::Formatters::Pretty.new
    document = nil
    begin
      document = REXML::Document.new(xml)
    rescue REXML::ParseException
      skip_message = "skip this test because we can't check Pretty#wrap " +
        "works without #<SystemStackError: stack level too deep> on " +
        "small memory system. #<RegexpError: failed to allocate memory> " +
        "will be raised on the system. See also [ruby-dev:42599]."
      return skip_message
    end
    output = ""
    formatter.write(document, output)
    assert_equal("<doc>\n" +
                 ((" " + (" aaaa" * 15) + "\n") * (n / 15)) +
                 "  " + ("aaaa " * (n % 15)) + "\n" +
                 "</doc>",
                 output)
  end

  def test_pretty_format_deep_indent
    n = 6
    elements = ""
    n.times do |i|
      elements << "<element#{i}>"
      elements << "element#{i} " * 5
    end
    (n - 1).downto(0) do |i|
      elements << "</element#{i}>"
    end
    xml = "<doc>#{elements}</doc>"
    document = REXML::Document.new(xml)
    formatter = REXML::Formatters::Pretty.new
    formatter.width = 20
    output = ""
    formatter.write(document, output)
    assert_equal(<<-XML.strip, output)
<doc>
  <element0>
    element0
    element0
    element0
    element0
    element0 
    <element1>
      element1
      element1
      element1
      element1
      element1 
      <element2>
        element2
        element2
        element2
        element2
        element2 
        <element3>
          element3
          element3
          element3
          element3
          element3 
          <element4>
            element4
            element4
            element4
            element4
            element4
            
            <element5>
              element5 element5 element5 element5 element5 
            </element5>
          </element4>
        </element3>
      </element2>
    </element1>
  </element0>
</doc>
    XML
  end

  def test_ticket_58
    doc = REXML::Document.new
    doc << REXML::XMLDecl.default
    doc << REXML::Element.new("a")

    str = ""
    doc.write(str)

    assert_equal("<a/>", str)

    doc = REXML::Document.new
    doc << REXML::XMLDecl.new("1.0", "UTF-8")
    doc << REXML::Element.new("a")

    str = ""
    doc.write(str)

    assert_equal("<?xml version='1.0' encoding='UTF-8'?><a/>", str)
  end

  # Incomplete tags should generate an error
  def test_ticket_53
    assert_raise( REXML::ParseException ) {
      REXML::Document.new( "<a><b></a>" )
    }
    assert_raise( REXML::ParseException ) {
      REXML::Document.new( "<a><b>" )
    }
    assert_raise( REXML::ParseException ) {
      REXML::Document.new( "<a><b/>" )
    }
  end

  def test_ticket_52
    source = "<!-- this is a single line comment -->"
    d = REXML::Document.new(source)
    d.write(k="")
    assert_equal( source, k )

    source = "<a><!-- Comment --></a>"
    target = "<a>\n    <!-- Comment -->\n</a>"
    d = REXML::Document.new(source)
    REXML::Formatters::Pretty.new(4).write(d,k="")
    assert_equal( target, k )
  end

  def test_ticket_76
    src = "<div>at&t"
    assert_raise( ParseException, %Q{"#{src}" is invalid XML} )  {
      REXML::Document.new(src)
    }
  end

  def test_ticket_21
    src = "<foo bar=value/>"
    assert_raise( ParseException, "invalid XML should be caught" ) {
      Document.new(src)
    }
    begin
      Document.new(src)
    rescue
      assert_match( /missing attribute quote/, $!.message )
    end
  end

  def test_ticket_63
    Document.new(File.new(fixture_path("t63-1.xml")))
  end

  def test_ticket_75
    d = REXML::Document.new(File.new(fixture_path("t75.xml")))
    assert_equal("tree", d.root.name)
  end

  def test_ticket_48_part_II
    f = REXML::Formatters::Pretty.new
    #- rexml sanity check (bugs in ruby 1.8.4, ruby 1.8.6)
    xmldoc = Document.new("<test/>")
    xmldoc << XMLDecl.new(XMLDecl::DEFAULT_VERSION, "UTF-8")
    content = ['61c3a927223c3e26'].pack("H*")
    content.force_encoding('UTF-8') if content.respond_to?(:force_encoding)
    #- is some UTF-8 text but just to make sure my editor won't magically convert..
    xmldoc.root.add_attribute('attr', content)
    f.write(xmldoc,out=[])

    xmldoc = REXML::Document.new(out.join)
    sanity1 = xmldoc.root.attributes['attr']
    f.write(xmldoc,out=[])

    xmldoc = REXML::Document.new(out.join)
    sanity2 = xmldoc.root.attributes['attr']
    f.write(xmldoc,out=[])

    assert_equal( sanity1, sanity2 )
  end

  def test_ticket_88
    doc = REXML::Document.new("<?xml version=\"1.0\" encoding=\"shift_jis\"?>")
    assert_equal("<?xml version='1.0' encoding='SHIFT_JIS'?>", doc.to_s)
    doc = REXML::Document.new("<?xml version = \"1.0\" encoding = \"shift_jis\"?>")
    assert_equal("<?xml version='1.0' encoding='SHIFT_JIS'?>", doc.to_s)
  end

  def test_ticket_85
    xml = <<ENDXML
<foo>
  <bar>
    <bob name='jimmy'/>
  </bar>
</foo>
ENDXML

    yml = "<foo>
  <bar>
    <bob name='jimmy'/>
  </bar>
</foo>"

    # The pretty printer ignores all whitespace, anyway so output1 == output2
    f = REXML::Formatters::Pretty.new( 2 )
    d = Document.new( xml, :ignore_whitespace_nodes=>:all )
    f.write( d, output1="" )

    d = Document.new( xml )
    f.write( d, output2="" )

    # Output directives should override whitespace directives.
    assert_equal( output1, output2 )

    # The base case.
    d = Document.new(yml)
    f.write( d, output3="" )

    assert_equal( output3.strip, output2.strip )

    d = Document.new(yml)
    f.write( d, output4="" )

    assert_equal( output3.strip, output4.strip )
  end

  def test_ticket_91
    source="<root>
      <bah something='1' somethingelse='bah'>
        <something>great</something>
      </bah>
    </root>"
    expected="<root>
  <bah something='1' somethingelse='bah'>
    <something>great</something>
  </bah>
  <bah/>
</root>"
    d = Document.new( source )
    d.root.add_element( "bah" )
    p=REXML::Formatters::Pretty.new(2)
    p.compact = true    # Don't add whitespace to text nodes unless necessary
    p.write(d,out="")
    assert_equal( expected, out )
  end

  def test_ticket_95
    testd = REXML::Document.new "<a><b><c/><c/><c/></b></a>"
    testd.write(out1="")
    testd.elements["//c[2]"].xpath
    testd.write(out2="")
    assert_equal(out1,out2)
  end

  def test_ticket_102
    doc = REXML::Document.new '<doc xmlns="ns"><item name="foo"/></doc>'
    assert_equal( "foo", doc.root.elements["item"].attribute("name","ns").to_s )
    assert_equal( "item", doc.root.elements["item[@name='foo']"].name )
  end

  def test_ticket_14
    # Per .2.5 Node Tests of XPath spec
    assert_raise( REXML::UndefinedNamespaceException,
                   %Q{Should have gotten an Undefined Namespace error} )  {
      Document.new("<a><n:b/></a>")
    }
  end

  # 5.7 Text Nodes
  # Character data is grouped into text nodes. As much character data as
  # possible is grouped into each text node: a text node never has an
  # immediately following or preceding sibling that is a text node. The
  # string-value of a text node is the character data. A text node always has
  # at least one character of data.
  def test_ticket_105
    d = Document.new("<a/>")
    d.root.add_text( "a" )
    d.root.add_text( "b" )
    assert_equal( 1, d.root.children.size )
  end

  # phantom namespace same as default namespace
  def test_ticket_121
    doc = REXML::Document.new(
      '<doc xmlns="ns" xmlns:phantom="ns"><item name="foo">text</item></doc>'
    )
    assert_equal 'text', doc.text( "/doc/item[@name='foo']" )
    assert_equal "name='foo'",
      doc.root.elements["item"].attribute("name", "ns").inspect
    assert_equal "<item name='foo'>text</item>",
      doc.root.elements["item[@name='foo']"].to_s
  end

  def test_ticket_135
    bean_element = REXML::Element.new("bean")
    textToAdd = "(&#38;(|(memberof=CN=somegroupabcdefgh,OU=OUsucks,DC=hookemhorns,DC=com)(mail=*someco.com))(acct=%u)(!(extraparameter:2.2.222.222222.2.2.222:=2)))"
    bean_element.add_element("prop", {"key"=> "filter"}).add_text(textToAdd)
    doc = REXML::Document.new
    doc.add_element(bean_element)

    REXML::Formatters::Pretty.new(3).write( doc, out = "" )

    assert_equal "<bean>\n   <prop key='filter'>\n      (&amp;#38;(|(memberof=CN=somegroupabcdefgh,OU=OUsucks,DC=hookemhorns,DC=com)(mail=*someco.com))(acct=%u)(!(extraparameter:2.2.222.222222.2.2.222:=2)))\n   </prop>\n</bean>", out
  end

  def test_ticket_138
    doc = REXML::Document.new(
      '<svg xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape" ' +
         'inkscape:version="0.44" version="1.0"/>'
    )
    expected = {
      "inkscape" => attribute("xmlns:inkscape",
                              "http://www.inkscape.org/namespaces/inkscape"),
      "version" => {
        "inkscape" => attribute("inkscape:version", "0.44"),
        "" => attribute("version", "1.0"),
      },
    }
    assert_equal(expected, doc.root.attributes)
    assert_equal(expected, REXML::Document.new(doc.root.to_s).root.attributes)
  end

  def test_empty_doc
    assert(REXML::Document.new('').children.empty?)
  end

  private
  def attribute(name, value)
    REXML::Attribute.new(name, value)
  end
end
