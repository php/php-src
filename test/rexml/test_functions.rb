require "test/unit/testcase"

require "rexml/document"

class FunctionsTester < Test::Unit::TestCase
  include REXML
  def test_functions
    # trivial text() test
    # confuse-a-function
    source = "<a>more <b id='1'/><b id='2'>dumb</b><b id='3'/><c/> text</a>"
    doc = Document.new source
    res = ""
    XPath::each(doc.root, "text()") {|val| res << val.to_s}
    assert_equal "more  text", res

    res = XPath::first(doc.root, "b[last()]")
    assert_equal '3', res.attributes['id']
    res = XPath::first(doc.root, "b[position()=2]")
    assert_equal '2', res.attributes['id']
    res = XPath::first(doc.root, "*[name()='c']")
    assert_equal "c", res.name
  end

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

    [['o', 2], ['foo', 1], ['bar', 0]].each { |test|
      search, expected = test
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

  def test_substring_angrez
    testString = REXML::Functions::substring_after("helloworld","hello")
    assert_equal( 'world', testString )
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

  def test_name
    d = REXML::Document.new("<a xmlns:x='foo'><b/><x:b/></a>")
    assert_equal 1, d.root.elements.to_a('*[name() = "b"]').size
    assert_equal 1, d.elements.to_a('//*[name() = "x:b"]').size
  end

  def test_local_name
    d = REXML::Document.new("<a xmlns:x='foo'><b/><x:b/></a>")
    assert_equal 2, d.root.elements.to_a('*[local_name() = "b"]').size
    assert_equal 2, d.elements.to_a('//*[local_name() = "b"]').size
  end

  def test_substring2
    doc = Document.new('<test string="12345" />')
    assert_equal(1,doc.elements.to_a("//test[substring(@string,2)='2345']").size)
  end

  # Submitted by Kouhei
  def test_floor_ceiling_round
    source = "<a><b id='1'/><b id='2'/><b id='3'/></a>"
    doc = REXML::Document.new(source)

    id_1 = doc.elements["/a/b[@id='1']"]
    id_2 = doc.elements["/a/b[@id='2']"]
    id_3 = doc.elements["/a/b[@id='3']"]

    good = {
      "floor" => [[], [id_1], [id_2], [id_3]],
      "ceiling" => [[id_1], [id_2], [id_3], []],
      "round" => [[id_1], [id_2], [id_3], []]
    }
    good.each do |key, value|
      (0..3).each do |i|
        xpath = "//b[number(@id) = #{key}(#{i+0.5})]"
        assert_equal(value[i], REXML::XPath.match(doc, xpath))
      end
    end

    good["round"] = [[], [id_1], [id_2], [id_3]]
    good.each do |key, value|
      (0..3).each do |i|
        xpath = "//b[number(@id) = #{key}(#{i+0.4})]"
        assert_equal(value[i], REXML::XPath.match(doc, xpath))
      end
    end
  end

  # Submitted by Kou
  def test_lang
    d = Document.new(<<-XML)
    <a xml:lang="en">
    <b xml:lang="ja">
    <c xml:lang="fr"/>
    <d/>
    <e xml:lang="ja-JP"/>
    <f xml:lang="en-US"/>
    </b>
    </a>
    XML

    assert_equal(1, d.elements.to_a("//*[lang('fr')]").size)
    assert_equal(3, d.elements.to_a("//*[lang('ja')]").size)
    assert_equal(2, d.elements.to_a("//*[lang('en')]").size)
    assert_equal(1, d.elements.to_a("//*[lang('en-us')]").size)

    d = Document.new(<<-XML)
    <root>
    <para xml:lang="en"/>
    <div xml:lang="en"><para/></div>
    <para xml:lang="EN"/>
    <para xml:lang="en-us"/>
    </root>
    XML

    assert_equal(5, d.elements.to_a("//*[lang('en')]").size)
  end

  def test_ticket_60
    document = REXML::Document.new("<a><b>A</b><b>1</b></a>")
    assert_equal( "A", REXML::XPath.first(document, '//b[.="A"]').text )
    assert_equal( "1", REXML::XPath.first(document, '//b[.="1"]').text )
  end

  def test_normalize_space
    source = "<a><!--COMMENT A--><b><!-- COMMENT A --></b></a>"
    doc = REXML::Document.new(source)
    predicate = "string(.)=normalize_space('\nCOMMENT    \n A \n\n ')"
    m = REXML::XPath.match(doc, "//comment()[#{predicate}]")
    assert_equal( [REXML::Comment.new("COMMENT A")], m )
  end
end
