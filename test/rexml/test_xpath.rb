require "rexml_test_utils"

require "rexml/document"

class XPathTester < Test::Unit::TestCase
  include REXMLTestUtils
  include REXML
  SOURCE = <<-EOF
    <a id='1'>
      <b id='2' x='y'>
        <c id='3'/>
        <c id='4'/>
      </b>
      <d id='5'>
        <c id='6' x='y'/>
        <c id='7'/>
        <c id='8'/>
        <q id='19'/>
      </d>
      <e id='9'>
        <f id='10' a='b'/>
        <f id='11' a='c'/>
        <f id='12' a='d'>
          <g id='13'/>
        </f>
        <f id='14' a='d'/>
      </e>
      <m id='15'>
        <n id='16'>
          <o id='17'>
            <p id='18'/>
          </o>
        </n>
      </m>
    </a>
    EOF
  JENI_TENNISON = <<-EOJT
  <a>
    <b>
      <c>
        <d>
          <e id='x'>
            <f/>
          </e>
        </d>
      </c>
      <c>
        <d>
          <e id='y'/>
        </d>
      </c>
    </b>
    <b>
      <c>
        <d>
          <e id='z'/>
        </d>
      </c>
    </b>
  </a>
  EOJT

  def setup
    @@doc = Document.new(SOURCE) unless defined? @@doc
    @@jeni = Document.new( JENI_TENNISON ) unless defined? @@jeni
  end

  def each_test( element, xpath )
    count = 0
    XPath::each( element, xpath ) { |child|
      count += 1
      yield child if block_given?
    }
    count
  end

  def test_descendant
    doc = Document.new("<a><b><c id='1'/></b><d><b><c id='2'/></b></d></a>")
    p = XPath.match( doc, "//c" )
    assert_equal( 2, p.size )
    p = XPath.first( @@doc, "//p" )
    assert_equal "p", p.name
    c = each_test( @@doc, "//c" ) { |child| assert_equal "c", child.name }
    assert_equal 5, c
    c = each_test( @@doc.root, "b//c" ) { |child|
        assert_equal "c", child.name
    }
    assert_equal 2, c

    doc = Document.new( "<a><z id='1'/><b><z id='11'/><z id='12'/></b><c><z id='21'/><z id='22'/><d><z id='31'/><z id='32'/></d></c></a>" )
    # //para[1] : all descendants which are the first para child of their parent
    assert_equal( 4, XPath.match( doc, "//z[1]" ).size )
    # /descendant::para[1] : the first descendant para element
    assert_equal( 1, XPath.match( doc, "/descendant::z[1]" ).size )
  end

  def test_root
    source = "<a><b/></a>"
    doc = Document.new( source )
    assert_equal doc, doc.root_node
    assert_equal "a", XPath::first( doc, "/" ).elements[1].name
  end

  def test_abbreviated_simple_child
    assert_equal "a", XPath::first(@@doc, "a").name
  end

  def test_child
    c = XPath::first( @@doc, "a/b/c" )
    assert_equal "c", c.name
    assert_equal "3", XPath::first(@@doc, "a/b/c").attributes["id"]
  end

  def test_root_child
    assert_equal "a", XPath::first(@@doc, "/a").name
    c = XPath::first( @@doc, "a/b/c" )
    assert_equal "a", XPath::first(c, "/a").name
  end

  def test_root_children
    c = XPath::first( @@doc, "a/b/c" )
    assert_equal "2", XPath::first(c, "/a/b").attributes["id"]
  end

  def test_abbreviated_step
    c = XPath::first( @@doc, "a/b/c" )
    assert_equal("c", c.name)
    assert_equal("a", XPath::first(@@doc.root, ".").name)
    assert_equal("b", XPath::first(c, "..").name)
    assert_equal("a", XPath::first(@@doc, "a/b/..").name)

    doc = REXML::Document.new(File.new(fixture_path("project.xml")))
    c = each_test(doc.root, "./Description" ) { |child|
      assert_equal("Description",child.name)
    }
    assert_equal 1, c
  end

  # Things that aren't tested elsewhere
  def test_predicates
    assert_equal "12", XPath::first(@@doc, "a/e/f[3]").attributes["id"]
    assert_equal "13", XPath::first(@@doc, "a/e/f[3]/g").attributes["id"]
    assert_equal "14", XPath::first(@@doc, "a/e/f[@a='d'][2]").attributes["id"]
    assert_equal "14", XPath::first(@@doc, "a/e/f[@a='d'][@id='14']").attributes["id"]
    assert_equal "a", XPath::first( @@doc, "*[name()='a' and @id='1']" ).name
    c=each_test( @@doc, "//*[name()='f' and @a='d']") { |i|
      assert_equal "f", i.name
    }
    assert_equal 2, c
    c=each_test( @@doc, "//*[name()='m' or @a='d']") { |i|
      assert ["m","f"].include?(i.name)
    }
    assert_equal 3, c

    assert_equal "b", XPath::first( @@doc, "//b[@x]" ).name
  end

  def test_node_type
    doc = Document.new "<a><?foo bar?><!--comment-->text</a>"
    #res = XPath::first(doc.root, "text()")
    #assert_equal "text", res.to_s

    #res = XPath::first(doc, "*")
    #assert_equal "a", res.name

    assert_equal( :processing_instruction,
    XPath::first(doc.root, "processing-instruction()").node_type)
    assert_equal( :comment, XPath::first(doc.root, "comment()").node_type)
  end

  def test_functions
    # trivial text() test
    # confuse-a-function
    source = "<a>more <b id='1'/><b id='2'>dumb</b><b id='3'/><c/> text</a>"
    doc = Document.new source
    res = ""
    #XPath::each(doc.root, "text()") {|val| res << val.to_s}
    #assert_equal "more  text", res

    #res = XPath::first(doc.root, "b[last()]")
    #assert_equal '3', res.attributes['id']
    res = XPath::first(doc.root, "b[position()=2]")
    assert_equal '2', res.attributes['id']
    res = XPath::first(doc.root, "*[name()='c']")
    assert_equal "c", res.name
  end

  def no_test_ancestor
    doc = REXML::Document.new(File.new(fixture_path("testsrc.xml")))
    doc.elements.each("//item") { |el| print el.name
      if el.attributes['x']
        puts " -- "+el.attributes['x']
      else
        puts
      end
    }
    doc.elements.each("//item/ancestor::") { |el| print el.name
      if el.attributes['x']
        puts " -- "+el.attributes['x']
      else
        puts
      end
    }
  end

  # Here are some XPath tests that were originally submitted by ...
  # The code has changed some, but the logic and the source documents are the
  # same.
  # This method reads a document from a file, and then a series of xpaths,
  # also from a file.  It then checks each xpath against the source file.
  def test_more
    xmlsource   = fixture_path("testsrc.xml")
    xpathtests  = fixture_path("xp.tst")

    doc = REXML::Document.new(File.new(xmlsource))
    #results = ""
    results = REXML::Document.new
    results.add_element "test-results"
    for line in File.new(xpathtests)
      line.strip!
      begin
        rt = doc.root
        #puts "#"*80
        #print "\nDoing #{line} " ; $stdout.flush
        doc.elements.each(line) do |el|
          #print "." ; $stdout.flush
          results.root << el.clone
          #results << el.to_s
        end
        #ObjectSpace.garbage_collect
        GC::start
      rescue Exception => z
        #puts "\n'#{line}' failed"
        fail("Error on line #{line}:\n#{z.message}\n"+z.backtrace[0,10].join("\n"))
        #results.root.add_element( "error", {"path"=>line}).text = z.message+"\n"+z.backtrace[0,10].join("\n")
        #results << "<error path='"+line+"'>"+z.message+"</error>"
      end
    end
  end

  def test_axe_descendant
    assert_equal "f", XPath::first( @@doc, "descendant::f").name
  end

  def test_axe_parent
    q = XPath.first( @@doc, "a/d/c/parent::*/q" )
    assert_equal 19, q.attributes["id"].to_i
  end

  def test_abbreviated_attribute
    assert_equal 'a', XPath::first( @@doc, "a[@id='1']" ).name
    c = XPath::first( @@doc, "a/b/c[@id='4']" )
    assert_equal 'c', c.name
    assert_equal '4', c.attributes['id']

    result = XPath::first( @@doc, "descendant::f[@a='c']")
    assert_equal "11", result.attributes['id']

    assert_equal "11", XPath::first(@@doc, "a/e/f[@a='c']").attributes["id"]
    assert_equal "11", XPath::first(@@doc, "a/e/*[@a='c']").attributes["id"]
  end

  def test_axe_self
    c = XPath::first( @@doc, "a/b/c" )
    assert c
    assert_equal "c", c.name
    assert_equal "c", XPath::first( c, "self::node()" ).name
  end

  def test_axe_ancestor
    doc = REXML::Document.new "
    <a>
      <b id='1'>
        <c>
          <b id='2'>
            <d/>
          </b>
        </c>
      </b>
    </a>"

    d = XPath.first( doc, "//d" )
    assert_equal "d", d.name
    b = each_test( d, "ancestor::b" ) { |el|
      assert((1..2) === el.attributes['id'].to_i,
        "Expected #{el.attributes['id']} to be either 1 or 2"
      )
    }
    assert_equal 2, b
  end

  def test_axe_child
    m = XPath.first( @@doc, "a/child::m" )
    assert_equal 15, m.attributes['id'].to_i
  end

  def test_axe_attribute
    a = XPath.first( @@doc, "a/attribute::id" )
    assert_equal "1", a.value
    a = XPath.first( @@doc, "a/e/f[@id='14']/attribute::a" )
    assert_equal "d", a.value
  end

  def test_axe_sibling
    doc = Document.new "<a><b><c/></b><e><f id='10'/><f id='11'/><f id='12'/></e></a>"
    first_f = XPath.first( doc, "a/e/f" )
    assert first_f
    assert_equal '10', first_f.attributes['id']
    next_f = XPath.first( doc, "a/e/f/following-sibling::node()" )
    assert_equal '11', next_f.attributes['id']

    b = XPath.first( doc, "a/e/preceding-sibling::node()" )
    assert_equal 'b', b.name
  end

  def test_lang
    doc = Document.new(File.new(fixture_path("lang0.xml")))
    #puts IO.read( "test/lang.xml" )

    #puts XPath.match( doc, "//language/*" ).size
    c = each_test( doc, "//language/*" ) { |element|
      #puts "#{element.name}: #{element.text}"
    }
    assert_equal 4, c
  end

  def test_namespaces_1
    source = <<-EOF
      <foo xmlns:ts="this" xmlns:tt="that">
        <ts:bar>this bar</ts:bar>
        <tt:bar>that bar</tt:bar>
      </foo>
    EOF
    doc = Document.new source
    result = XPath.each( doc, "//bar" ) {
      fail "'bar' should match nothing in this case"
    }

    namespace = {"t"=>"this"}
    results = XPath.first( doc, "//t:bar", namespace )
    assert_equal "this bar", results.text
  end

  def test_namespaces_2
    source = <<-EOF
      <foo xmlns:ts="this" xmlns:tt="that">
        <ts:bar>this bar</ts:bar>
        <tt:bar>that bar</tt:bar>
      </foo>
    EOF
    doc = Document.new source
    res = XPath::first(doc, "//*[local_name()='bar']")
    assert res, "looking for //*[name()='bar']"
    assert_equal 'this', res.namespace
    res = XPath::first(doc.root, "*[namespace_uri()='that']")
    assert_equal 'that bar', res.text
  end

  def test_complex
    next_f = XPath.first( @@doc, "a/e/f[@id='11']/following-sibling::*" )
    assert_equal 12, next_f.attributes['id'].to_i
    prev_f = XPath.first( @@doc, "a/e/f[@id='11']/preceding-sibling::*" )
    assert_equal 10, prev_f.attributes['id'].to_i
    c = each_test( @@doc, "descendant-or-self::*[@x='y']" )
    assert_equal 2, c
  end

  def test_grouping
    t = XPath.first( @@doc, "a/d/*[name()='d' and (name()='f' or name()='q')]" )
    assert_nil t
    t = XPath.first( @@doc, "a/d/*[(name()='d' and name()='f') or name()='q']" )
    assert_equal 'q', t.name
  end

  def test_preceding
    d = Document.new "<a><b id='0'/><b id='2'/><b><c id='0'/><c id='1'/><c id='2'/></b><b id='1'/></a>"
    start = XPath.first( d, "/a/b[@id='1']" )
    assert_equal 'b', start.name
    c = XPath.first( start, "preceding::c" )
    assert_equal '2', c.attributes['id']

    c1, c0 = XPath.match( d, "/a/b/c[@id='2']/preceding::node()" )
    assert_equal '1', c1.attributes['id']
    assert_equal '0', c0.attributes['id']

    c2, c1, c0, b, b2, b0 = XPath.match( start, "preceding::node()" )

    assert_equal 'c', c2.name
    assert_equal 'c', c1.name
    assert_equal 'c', c0.name
    assert_equal 'b', b.name
    assert_equal 'b', b2.name
    assert_equal 'b', b0.name

    assert_equal '2', c2.attributes['id']
    assert_equal '1', c1.attributes['id']
    assert_equal '0', c0.attributes['id']
    assert b.attributes.empty?
    assert_equal '2', b2.attributes['id']
    assert_equal '0', b0.attributes['id']

    d = REXML::Document.new("<a><b/><c/><d/></a>")
    matches = REXML::XPath.match(d, "/a/d/preceding::node()")
    assert_equal("c", matches[0].name)
    assert_equal("b", matches[1].name)

    s = "<a><b><c id='1'/></b><b><b><c id='2'/><c id='3'/></b><c id='4'/></b><c id='NOMATCH'><c id='5'/></c></a>"
    d = REXML::Document.new(s)
    c = REXML::XPath.match( d, "//c[@id = '5']")
    cs = REXML::XPath.match( c, "preceding::c" )
    assert_equal( 4, cs.length )
  end

  def test_following
    d = Document.new "<a><b id='0'/><b/><b><c id='1'/><c id='2'/></b><b id='1'/></a>"
    start = XPath.first( d, "/a/b[@id='0']" )
    assert_equal 'b', start.name
    c = XPath.first( start, "following::c" )
    assert_equal '1', c.attributes['id']

    s = "<a><b><c><d/></c><e/></b><f><g><h/><i/></g></f><i/></a>"
    d = Document.new(s)
    c = XPath.first(d, '/a/b/c')
    assert_equal 'c', c.name
    res = XPath.match( c, 'following::*' )
    assert_equal 6, res.size
    res = XPath.match( c, 'following::i' )
    assert_equal 2, res.size
  end

  # The following three paths were provided by
  # Jeni Tennison <jeni@jenitennison.com>
  # a consultant who is also an XSL and XPath expert
  #def test_child_cubed
  #  els = @@jeni.elements.to_a("*****")
  #  assert_equal 3, els.size
  #end

  #def test_div_2
  #  results = doc.elements.to_a("/ div 2")
  #end

  #def test_nested_predicates
  #  puts @@jeni.root.elements[1].elements[1].name
  #  results = @@jeni.root.elements[1].elements[1].elements.to_a("../following-sibling::*[*[name() = name(current())]]")
  #  puts results
  #end

  # Contributed by Mike Stok
  def test_starts_with
    source = <<-EOF
      <foo>
      <a href="mailto:a@b.c">a@b.c</a>
      <a href="http://www.foo.com">http://www.foo.com</a>
      </foo>
    EOF
    doc = Document.new source
    mailtos = doc.elements.to_a("//a[starts-with(@href, 'mailto:')]")
    assert_equal 1, mailtos.size
    assert_equal "mailto:a@b.c", mailtos[0].attributes['href']

    ailtos = doc.elements.to_a("//a[starts-with(@href, 'ailto:')]")
    assert_equal 0, ailtos.size
  end

  def test_toms_text_node
    file = "<a>A<b>B</b><c>C<d>D</d>E</c>F</a>"
    doc = Document.new(file)
    assert_equal 'A', XPath.first(doc[0], 'text()').to_s
    assert_equal 'AF', XPath.match(doc[0], 'text()').collect { |n|
      n.to_s
    }.join('')
    assert_equal 'B', XPath.first(doc[0], 'b/text()').to_s
    assert_equal 'D', XPath.first(doc[0], '//d/text()').to_s
    assert_equal 'ABCDEF', XPath.match(doc[0], '//text()').collect {|n|
      n.to_s
    }.join('')
  end

  def test_string_length
    doc = Document.new <<-EOF
      <AAA>
      <Q/>
      <SSSS/>
      <BB/>
      <CCC/>
      <DDDDDDDD/>
      <EEEE/>
      </AAA>
    EOF
    assert doc, "create doc"

    set = doc.elements.to_a("//*[string-length(name()) = 3]")
    assert_equal 2, set.size, "nodes with names length = 3"

    set = doc.elements.to_a("//*[string-length(name()) < 3]")
    assert_equal 2, set.size, "nodes with names length < 3"

    set = doc.elements.to_a("//*[string-length(name()) > 3]")
    assert_equal 3, set.size, "nodes with names length > 3"
  end

  # Test provided by Mike Stok
  def test_contains
    source = <<-EOF
      <foo>
      <a href="mailto:a@b.c">a@b.c</a>
      <a href="http://www.foo.com">http://www.foo.com</a>
      </foo>
    EOF
    doc = Document.new source

    [
    #['o', 2],
    ['foo', 1], ['bar', 0]].each { |search, expected|
      set = doc.elements.to_a("//a[contains(@href, '#{search}')]")
      assert_equal expected, set.size
    }
  end

  # Mike Stok and Sean Russell
  def test_substring
    # examples from http://www.w3.org/TR/xpath#function-substring
    doc = Document.new('<test string="12345" />')

    d = Document.new("<a b='1'/>")
    #puts XPath.first(d, 'node()[0 + 1]')
    #d = Document.new("<a b='1'/>")
    #puts XPath.first(d, 'a[0 mod 0]')
    [ [1.5, 2.6, '234'],
      [0, 3, '12'],
      [0, '0 div 0', ''],
      [1, '0 div 0', ''],
      ['-42', '1 div 0', '12345'],
      ['-1 div 0', '1 div 0', '']
    ].each { |start, length, expected|
      set = doc.elements.to_a("//test[substring(@string, #{start}, #{length}) = '#{expected}']")
      assert_equal 1, set.size, "#{start}, #{length}, '#{expected}'"
    }
  end

  def test_translate
    source = <<-EOF
    <doc>
    <case name='w3c one' result='BAr' />        <!-- w3c -->
    <case name='w3c two' result='AAA' />        <!-- w3c -->
    <case name='alchemy' result="gold" />   <!-- mike -->
    <case name='vbxml one' result='A Space Odyssey' />
    <case name='vbxml two' result='AbCdEf' />
    </doc>
    EOF

    doc = Document.new(source)

    [ ['bar', 'abc', 'ABC', 'w3c one'],
    ['--aaa--','abc-','ABC', 'w3c two'],
    ['lead', 'dear language', 'doll groover', 'alchemy'],
    ['A Space Odissei', 'i', 'y', 'vbxml one'],
    ['abcdefg', 'aceg', 'ACE', 'vbxml two'],
    ].each { |arg1, arg2, arg3, name|
      translate = "translate('#{arg1}', '#{arg2}', '#{arg3}')"
      set = doc.elements.to_a("//case[@result = #{translate}]")
      assert_equal 1, set.size, translate
      assert_equal name, set[0].attributes['name']
    }
  end

  def test_math
    d = Document.new( '<a><b/><c/></a>' )
    assert XPath.first( d.root, 'node()[1]' )
    assert_equal 'b', XPath.first( d.root, 'node()[1]' ).name
    assert XPath.first( d.root, 'node()[0 + 1]' )
    assert_equal 'b', XPath.first( d.root, './node()[0 + 1]' ).name
    assert XPath.first( d.root, 'node()[1 + 1]' )
    assert_equal 'c', XPath.first( d.root, './node()[1 + 1]' ).name
    assert XPath.first( d.root, 'node()[4 div 2]' )
    assert_equal 'c', XPath.first( d.root, './node()[4 div 2]' ).name
    assert XPath.first( d.root, 'node()[2 - 1]' )
    assert_equal 'b', XPath.first( d.root, './node()[2 - 1]' ).name
    assert XPath.first( d.root, 'node()[5 mod 2]' )
    assert_equal 'b', XPath.first( d.root, './node()[5 mod 2]' ).name
    assert XPath.first( d.root, 'node()[8 mod 3]' )
    assert_equal 'c', XPath.first( d.root, './node()[8 mod 3]' ).name
    assert XPath.first( d.root, 'node()[1 * 2]' )
    assert_equal 'c', XPath.first( d.root, './node()[1 * 2]' ).name
    assert XPath.first( d.root, 'node()[2 + -1]' )
    assert_equal 'b', XPath.first( d.root, './node()[2 + -1]' ).name
  end

  def test_name
    assert_raise( UndefinedNamespaceException, "x should be undefined" ) {
      d = REXML::Document.new("<a x='foo'><b/><x:b/></a>")
    }
    d = REXML::Document.new("<a xmlns:x='foo'><b/><x:b/></a>")
    assert_equal 1, d.root.elements.to_a('*[name() = "b"]').size
    assert_equal 1, d.elements.to_a('//*[name() = "x:b"]').size
  end

  def test_local_name
    d = REXML::Document.new("<a xmlns:x='foo'><b/><x:b/></a>")
    assert_equal 2, d.root.elements.to_a('*[local_name() = "b"]').size
    assert_equal 2, d.elements.to_a('//*[local_name() = "b"]').size
  end

  def test_comparisons
    source = "<a><b id='1'/><b id='2'/><b id='3'/></a>"
    doc = REXML::Document.new(source)

    # NOTE TO SER: check that number() is required
    assert_equal 2, REXML::XPath.match(doc, "//b[number(@id) > 1]").size
    assert_equal 3, REXML::XPath.match(doc, "//b[number(@id) >= 1]").size
    assert_equal 1, REXML::XPath.match(doc, "//b[number(@id) <= 1]").size
    assert_equal 1, REXML::XPath.match(doc, "//b[number(@id) = (1 * 1)]").size
    assert_equal 1, REXML::XPath.match(doc, "//b[number(@id) = (1 mod 2)]").size
    assert_equal 1, REXML::XPath.match(doc, "//b[number(@id) = (4 div 2)]").size
  end

  # Contributed by Kouhei
  def test_substring_before
    doc = Document.new("<r><a/><b/><c/></r>")
    assert_equal("a", doc.root.elements.to_a("*[name()=substring-before('abc', 'b')]")[0].name)
    assert_equal("c", doc.root.elements.to_a("*[name()=substring-after('abc', 'b')]")[0].name)
  end

  def test_spaces
    doc = Document.new("<a>
      <b>
        <c id='a'/>
      </b>
      <c id='b'/>
    </a>")
    assert_equal( 1, REXML::XPath.match(doc,
      "//*[local-name()='c' and @id='b']").size )
    assert_equal( 1, REXML::XPath.match(doc,
      "//*[ local-name()='c' and @id='b' ]").size )
    assert_equal( 1, REXML::XPath.match(doc,
      "//*[ local-name() = 'c' and @id = 'b' ]").size )
    assert_equal( 1,
      REXML::XPath.match(doc, '/a/c[@id]').size )
    assert_equal( 1,
      REXML::XPath.match(doc, '/a/c[(@id)]').size )
    assert_equal( 1,
      REXML::XPath.match(doc, '/a/c[ @id ]').size )
    assert_equal( 1,
      REXML::XPath.match(doc, '/a/c[ (@id) ]').size )
    assert_equal( 1,
      REXML::XPath.match(doc, '/a/c[( @id )]').size )
    assert_equal( 1, REXML::XPath.match(doc.root,
      '/a/c[ ( @id ) ]').size )
    assert_equal( 1, REXML::XPath.match(doc,
      '/a/c [ ( @id ) ] ').size )
    assert_equal( 1, REXML::XPath.match(doc,
      ' / a / c [ ( @id ) ] ').size )
  end

  def test_text_nodes
    #  source = "<root>
    #<child/>
    #<child>test</child>
    #</root>"
    source = "<root><child>test</child></root>"
    d = REXML::Document.new( source )
    r = REXML::XPath.match( d, %q{/root/child[text()="test"]} )
    assert_equal( 1, r.size )
    assert_equal( "child", r[0].name )
    assert_equal( "test", r[0].text )
  end

  def test_auto_string_value
    source = "<root><foo/><title>Introduction</title></root>"
    d = REXML::Document.new( source )
    #r = REXML::XPath.match( d, %q{/root[title="Introduction"]} )
    #assert_equal( 1, r.size )
    source = "<a><b/><c/><c>test</c></a>"
    d = REXML::Document.new( source )
    r = REXML::XPath.match( d, %q{/a[c='test']} )
    assert_equal( 1, r.size )
    r = REXML::XPath.match( d, %q{a[c='test']} )
    assert_equal( 1, r.size )
    r = d.elements["/a[c='test']"]
    assert_not_nil( r )
    r = d.elements["a[c='test']"]
    assert_not_nil( r )
    r = d.elements["a[c='xtest']"]
    assert_nil( r )
    r = REXML::XPath.match( d, %q{a[c='xtest']} )
    assert_equal( 0, r.size )
  end

  def test_ordering
    source = "<a><b><c id='1'/><c id='2'/></b><b><d id='1'/><d id='2'/></b></a>"
    d = REXML::Document.new( source )
    r = REXML::XPath.match( d, %q{/a/*/*[1]} )
    assert_equal( 1, r.size )
    r.each { |el| assert_equal( '1', el.attribute('id').value ) }
  end

  def test_descendant_or_self_ordering
    source = "<a>
    <b>
      <c id='1'/>
      <c id='2'/>
    </b>
    <b>
      <d id='1'>
        <c id='3'/>
      </d>
      <d id='2'>
        <e>
          <c id='4'/>
        </e>
      </d>
    </b>
  </a>"
    d = REXML::Document.new( source )
    cs = XPath.match( d, "/descendant-or-self::c" )
    assert_equal( 4, cs.length )
    1.upto(4) {|x| assert_equal( x.to_s, cs[x-1].attributes['id'] ) }
  end

  def test_and
    d = Document.new %q{<doc><route run='*' title='HNO'
    destination='debian_production1' date='*' edition='*'
    source='debian_satellite1'/></doc>}
    assert_equal( nil, d.root.elements["route[@run='0']"] )
    assert_equal( nil, d.root.elements["route[@run='0' and @title='HNO']"] )
  end


  def test_numbers
    d = Document.new %q{<a x="0" y="*" z="4e" w="e4" v="a"/>}

    xp1 = "/a[ @x = 0 ]"
    xp2 = "/a[ @x = '0' ]"
    xp3 = "/a[ (@x + 1) = 1 ]"
    xp4 = "/a[ @y = 0 ]"
    xp5 = "/a[ (@z + 1) = 5 ]"
    xp6 = "/a[ (@w + 1) = 5 ]"
    xp7 = "/a[ (@v + 1) = 1 ]"
    xp8 = "/a[ @n = 0 ]"

    assert_equal( 1, XPath.match( d, xp1 ).length )
    assert_equal( 1, XPath.match( d, xp2 ).length )
    assert_equal( 1, XPath.match( d, xp3 ).length )
    assert_equal( 0, XPath.match( d, xp4 ).length )
    assert_equal( 0, XPath.match( d, xp5 ).length )
    assert_equal( 0, XPath.match( d, xp6 ).length )
    assert_equal( 0, XPath.match( d, xp7 ).length )
    assert_equal( 0, XPath.match( d, xp8 ).length )
  end

  def test_tobis_preceding
    doc_string = '<a>
  <b/>
  <c>
    <d/>
    <e/>
  </c>
</a>'

    doc = Document.new(doc_string)

    # e = REXML::XPath.first(doc,'/a/c/e')
    e = doc.root.get_elements('/a/c/e')[0]
    assert_equal( 1, e.get_elements('preceding-sibling::*').length )
    assert_equal( 2, XPath.match(e, 'preceding::*').length )
  end


  def test_filtering
    #doc=Document.new("<a><b><c1/><c2/></b><b><c3/><c4/></b><b><c5/><c6/></b></a>")
    #assert_equal( 3, XPath.match( doc, '/a/b/*[1]' ).length )
    #assert_equal( 2, XPath.match( doc, '/a/b/following-sibling::*[1]' ).length )
  end

  # Submitted by Alex
  def test_union
    data = %Q{<div id="the_div">
  <span id="the_span">
  <strong id="the_strong">a</strong>
  </span>
  <em id="the_em2">b</em>
</div>}
    rd = REXML::Document.new( data )
    #union = rd.get_elements("/div/span | /div/em")
    #assert_equal(2, union.length, "/div/span | /div/em" )
    union = rd.get_elements('//*[name()="em" or name()="strong"]')
    assert_equal(2, union.length, 'name() and "or" failed')
    union = rd.get_elements('//em|//strong')
    assert_equal(2, union.length,
           'Both tag types are returned by XPath union operator')
  end


  def test_union2
    src = <<-EOL
<div id="the_div">
<span id="the_span">
<strong id="the_strong">a</strong>
</span>
<em id="the_em2">b</em>
</div>
    EOL
    rd = REXML::Document.new( src )
    union = rd.get_elements('//em|//strong')
    assert_equal(2, union.length,
      'Both tag types are returned by XPath union operator')
  end


  def test_a_star_star_one
    string = <<-EOL
<a>
  <b>
    <c1/>
    <d/>
    <e/>
    <f/>
  </b>
  <b>
    <c2/>
    <d/>
    <e/>
    <f/>
  </b>
</a>
    EOL
    d = REXML::Document.new( string )
    c1 = XPath.match( d, '/a/*/*[1]' )
    assert_equal( 1, c1.length )
    assert_equal( 'c1', c1[0].name )
  end

  def test_sum
    d = Document.new("<a>"+
    "<b>1</b><b>2</b><b>3</b>"+
    "<c><d>1</d><d>2</d></c>"+
    "<e att='1'/><e att='2'/>"+
    "</a>")

    for v,p in [[6, "sum(/a/b)"],
      [9, "sum(//b | //d)"],
      [3, "sum(/a/e/@*)"] ]
      assert_equal( v, XPath::match( d, p ).first )
    end
  end

  def test_xpath_namespace
    d = REXML::Document.new("<tag1 xmlns='ns1'><tag2 xmlns='ns2'/><tada>xa</tada></tag1>")
    x = d.root
    num = 0
    x.each_element('tada') {  num += 1 }
    assert_equal(1, num)
  end

  def test_ticket_39
    doc = REXML::Document.new( <<-EOL )
    <rss>
      <channel>
        <!-- removing the namespace declaration makes the test pass -->
        <convertLineBreaks xmlns="http://www.blogger.com/atom/ns#">true</convertLineBreaks>
        <item>
          <title>Item 1</title>
        </item>
        <item>
          <title>Item 2</title>
          <pubDate>Thu, 13 Oct 2005 19:59:00 +0000</pubDate>
        </item>
        <item>
          <title>Item 3</title>
        </item>
      </channel>
    </rss>
    EOL
    root_node = XPath.first(doc, "rss")
    assert_not_nil root_node
    channel_node = XPath.first(root_node, "channel")
    assert_not_nil channel_node
    items = XPath.match(channel_node, "*")
    assert_equal 4, items.size
    items = XPath.match(channel_node, "item")
    assert_equal 3, items.size  # fails
  end


  def test_ticket_42
    source = "<a></a>"
    doc = Document.new(source)
    bElem = Element.new('b')
    doc.root.add_element(bElem)
    doc.elements.each('//b[name(..) = "a"]') { |x|
      assert_equal x,bElem
    }
  end

  def test_ticket_56
    namespaces = {'h' => 'http://www.w3.org/1999/xhtml'}

    finaldoc = REXML::Document.new(File.read(fixture_path('google.2.xml')))

    column_headers = []

    REXML::XPath.each(finaldoc, '//h:form[@action="ModifyCampaign"]//h:th',
                      namespaces) do |el|
      node = REXML::XPath.first(el, 'h:a/text()', namespaces)
      column_headers << (node ? node.value : nil)
    end
    column_headers.map! { |h| h.to_s.strip.chomp }
    expected = ["", "", "Current Status", "Current Budget",
      "Clicks", "Impr.", "CTR", "Avg. CPC", "Cost", "Conv. Rate",
      "Cost/Conv."]
    assert_equal( expected, column_headers )
  end


  def test_ticket_70
    string = <<EOF

<mydoc>

    <someelement attribute="1.10">Text1, text,
text</someelement>

    <someelement attribute="1.11">Text2, text,
text</someelement>


</mydoc>

EOF
    doc = Document.new string
    assert_equal( 1, XPath.match( doc, "//someelement[contains(@attribute,'1.10')]" ).length )
  end

  def test_ticket_43
    #url = http://news.search.yahoo.com/news/rss?p=market&ei=UTF-8&fl=0&x=wrt

    sum = Document.new(File.new(fixture_path("yahoo.xml"))).elements.to_a("//item").size
    assert_equal( 10, sum )

    text = Document.new(File.new(fixture_path("yahoo.xml"))).elements.to_a(%Q{//title[contains(text(), "'")]}).collect{|e| e.text}.join
    assert_equal( "Broward labor market's a solid performer (Miami Herald)", text )
  end

  def test_ticket_57
    data = "<?xml version='1.0'?><a:x xmlns:a='1'><a:y p='p' q='q'><a:z>zzz</a:z></a:y></a:x>"

    r = Document.new(data)

    assert_equal(Text, REXML::XPath.first(r,"a:x/a:y[@p='p' and @q='q']/a:z/text()").class)
    assert_equal("zzz", REXML::XPath.first(r,"a:x/a:y[@p='p' and @q='q']/a:z/text()").to_s)
  end

  def test_ticket_59
    data = "<a>
      <c id='1'/>
      <c id='2'/>
      <b>
        <c id='3'/>
      </b>
      <c id='4'/>
      <b>
        <b>
          <c id='5'/>
        </b>
        <c id='6'/>
      </b>
      <c id='7'/>
      <b>
        <b>
          <c id='8'/>
          <b>
            <c id='9'/>
            <b>
              <c id='10'/>
            </b>
            <c id='11'/>
          </b>
        </b>
      </b>
      <c id='12'/>
    </a>"
    d = Document.new(data)
    res = d.elements.to_a( "//c" ).collect {|e| e.attributes['id'].to_i}
    assert_equal( res, res.sort )
  end

  def ticket_61_fixture(doc, xpath)
    matches = []
    doc.elements.each(xpath) do |element|
      matches << element
      assert_equal('Add', element.text)
      assert_equal('ButtonText', element.attributes['class'])
    end
    assert_equal(1, matches.length)
  end

  def test_ticket_61_text
    file = File.open(fixture_path("ticket_61.xml"))
    doc = REXML::Document.new file
    ticket_61_fixture( doc, "//div[text()='Add' and @class='ButtonText']" )
  end

  def test_ticket_61_contains
    file = File.open(fixture_path("ticket_61.xml"))
    doc = REXML::Document.new file
    ticket_61_fixture( doc, "//div[contains(.,'Add') and @class='ButtonText']" )
  end

  def test_namespaces_0
    d = Document.new(%q{<x:a xmlns:x="y"/>})
    assert_equal( 1,  XPath.match( d, "//x:a" ).size )
    assert_equal( 1,  XPath.match( d, "//x:*" ).size )
  end

  def test_ticket_71
    doc = Document.new(%Q{<root xmlns:ns1="xyz" xmlns:ns2="123"><element ns1:attrname="foo" ns2:attrname="bar"/></root>})
    el = doc.root.elements[1]
    assert_equal( "element", el.name )
    el2 = XPath.first( doc.root, "element[@ns:attrname='foo']", { 'ns' => "xyz" } )
    assert_equal( el, el2 )
  end

  def test_ticket_78
    doc = <<-EOT
    <root>
        <element>
            <tag x='1'>123</tag>
        </element>
        <element>
            <tag x='2'>123a</tag>
        </element>
    </root>
    EOT
    seq = %w{BEGIN 123 END BEGIN 123a END}

    xmlDoc = Document.new(doc)

    ["//element[tag='123']/tag", "//element[tag='123a']/tag"].each do |query|
      assert_equal( "BEGIN", seq.shift )
      XPath.each(xmlDoc, query) { |element|
        assert_equal( seq.shift, element.text )
      }
      assert_equal( "END", seq.shift )
    end
  end

  def test_ticket_79
    source = "<a><b><c>test</c></b><b><c>3</c></b></a>"
    d = REXML::Document.new( source )
    r = REXML::XPath.match( d, %q{/a/b[c='test']} )
    assert_equal(1, r.size())
    r = REXML::XPath.match( d, %q{/a/b[c='3']} )
    assert_equal(1, r.size())
  end

  def test_or_and
    doc = "
<html>
  <head>
    <title>test</title>
  </head>
  <body>
    <p>
      A <a rel=\"sub\" href=\"/\">link</a>.
    </p>
  </body>
</html>
"

    xmldoc = REXML::Document.new(doc)
    xpath = "descendant::node()[(local-name()='link' or local-name()='a') and @rel='sub']"
    hrefs = []
    xmldoc.elements.each(xpath) do |element|
      hrefs << element.attributes["href"]
    end
    assert_equal(["/"], hrefs, "Bug #3842 [ruby-core:32447]")
  end
end
