# coding: binary

require "rexml_test_utils"

require "rexml/document"
require "rexml/parseexception"
require "rexml/formatters/default"

class ContribTester < Test::Unit::TestCase
  include REXMLTestUtils
  include REXML

  XML_STRING_01 = <<DELIMITER
<?xml version="1.0" encoding="UTF-8"?>
<biblio>
  <entry type="Book">
    <author>Thomas, David; Hunt, Andrew</author>
    <language>english</language>
    <publisher>Addison-Wesley</publisher>
    <title>Programming Ruby. The Pragmatic Programmer's Guide</title>
    <year>2000</year>
  </entry>
  <entry type="Book">
    <author>Blammo, Blah</author>
    <language>english</language>
    <publisher>Hubbabubba</publisher>
    <title>Foozboozer's Life</title>
    <type>Book</type>
    <year>2002</year>
  </entry>
</biblio>
DELIMITER

  XML_STRING_02 = <<DELIMITER
<biblio>
  <entry type="Book">
    <language>english</language>
    <publisher>Addison-Wesley</publisher>
    <title>Programming Ruby. The Pragmatic Programmer's Guide</title>
    <type>Book</type>
    <year>2000</year>
  </entry>
  <entry type="Book">
    <author>Blammo, Blah</author>
    <language>english</language>
    <publisher>Hubbabubba</publisher>
    <title>Foozboozer's Life</title>
    <type>Book</type>
    <year>2002</year>
  </entry>
</biblio>
DELIMITER

  # Tobias Reif <tobiasreif@pinkjuice.com>
  def test_bad_doctype_Tobias
    source = <<-EOF
     <!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN"
        "http://www.w3.org/TR/SVG/DTD/svg10.dtd"
       [
       <!-- <!ENTITY % fast-slow "0 0  .5 1">-->
       <!--<!ENTITY % slow-fast ".5 0  1 1">-->
       <!ENTITY hover_ani
        '<animateTransform attributeName="transform"
         type="scale" restart="whenNotActive" values="1;0.96"
         dur="0.5s" calcMode="spline" keySplines="0 0  .5 1"
         fill="freeze" begin="mouseover"/>
         <animateTransform  attributeName="transform"
         type="scale" restart="whenNotActive" values="0.96;1"
         dur="0.5s" calcMode="spline" keySplines=".5 0  1 1"
         fill="freeze" begin="mouseover+0.5s"/>'
       >
       ]
     >
    EOF
    doc = REXML::Document.new source
    doc.write(out="")
    assert(out[/>\'>/] != nil, "Couldn't find >'>")
    assert(out[/\]>/] != nil, "Couldn't find ]>")
  end

  # Peter Verhage
  def test_namespace_Peter
    source = <<-EOF
    <?xml version="1.0"?>
    <config:myprog-config xmlns:config="http://someurl/program/version">
    <!-- main options -->
      <config:main>
        <config:parameter name="name"  value="value"/>
      </config:main>
    </config:myprog-config>
    EOF
    doc = REXML::Document.new source
    assert_equal "myprog-config", doc.root.name
    count = 0
    REXML::XPath.each(doc, "x:myprog-config/x:main/x:parameter",
      {"x"=>"http://someurl/program/version"}) { |element|
        assert_equal "name", element.attributes["name"]
      count += 1;
    }
    assert_equal 1, count
    assert_equal "myprog-config", doc.elements["config:myprog-config"].name
  end

  # Tobias Reif <tobiasreif@pinkjuice.com>
  def test_complex_xpath_Tobias
    source = <<-EOF
    <root>
      <foo>
        <bar style="baz"/>
        <blah style="baz"/>
        <blam style="baz"/>
      </foo>
      <wax>
        <fudge>
          <noodle/>
        </fudge>
      </wax>
    </root>
    EOF
    # elements that have child elements
    #  but not grandchildren
    #  and not children that don't have a style attribute
    #  and not children that have a unique style attribute
    complex_path = "*[* "+
      "and not(*/node()) "+
      "and not(*[not(@style)]) "+
      "and not(*/@style != */@style)]"
    doc = REXML::Document.new source
    results = REXML::XPath.match( doc.root, complex_path )
    assert(results)
    assert_equal 1, results.size
    assert_equal "foo", results[0].name
  end

  # "Chris Morris" <chrismo@charter.net>
  def test_extra_newline_on_read_Chris
    text = 'test text'
    e = REXML::Element.new('Test')
    e.add_text(text)
    REXML::Formatters::Default.new.write(e,out="")

    doc = REXML::Document.new(out)
    outtext = doc.root.text

    assert_equal(text, outtext)
  end

  # Tobias Reif <tobiasreif@pinkjuice.com>
  def test_other_xpath_Tobias
    schema = <<-DELIM
    <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"
      elementFormDefault="qualified">
      <xs:element name="rect">
        <xs:complexType>
          <xs:attribute name="width" type="xs:byte" use="required"/>
          <xs:attribute name="height" type="xs:byte" use="required"/>
        </xs:complexType>
      </xs:element>
      <xs:element name="svg">
        <xs:complexType>
          <xs:sequence>
            <xs:element ref="rect"/>
          </xs:sequence>
        </xs:complexType>
      </xs:element>
    </xs:schema>
    DELIM

    doc = REXML::Document.new schema

    result = REXML::XPath.first(doc.root, 'xs:element[descendant::xs:element[@ref]]')
    assert result
    assert_equal "svg", result.attributes['name']
    result = REXML::XPath.first(doc, 'element[descendant::element[@ref]]')
    assert_nil result
  end

  #this first test succeeds, to check if stuff is set up correctly
  def test_xpath_01_TobiasReif
    doc = Document.new XML_STRING_01.dup
    desired_result = Document.new '<author>Thomas, David; Hunt, Andrew</author>'
    xpath = '//author'
    result = XPath.first(doc, xpath)
    assert_equal desired_result.to_s, result.to_s
  end

  def test_xpath_whitespace_TobiasReif
    # same as above, with whitespace in XPath
    doc = Document.new(XML_STRING_01.dup)
    desired_result = Document.new('<author>Thomas, David; Hunt, Andrew</author>')
    xpath = "\/\/author\n \n"
    result = XPath.first(doc, xpath)
    failure_message = "\n[[[TR: AFAIK, whitespace should be allowed]]]\n"
    assert_equal(desired_result.to_s, result.to_s, failure_message)
  end

  def test_xpath_02_TobiasReif
    doc = Document.new XML_STRING_01.dup
    desired_result = Document.new '<author>Thomas, David; Hunt, Andrew</author>'
    # Could that quirky
    #  Programmer',&quot;'&quot;,'s
    # be handled automatically, somehow?
    # Or is there a simpler way? (the below XPath should match the author element above,
    # AFAIK; I tested it inside an XSLT)
    xpath = %q{/biblio/entry[
    title/text()=concat('Programming Ruby. The Pragmatic Programmer',"'",'s Guide')
    and
    year='2000'
    ]/author}
    result = XPath.first(doc, xpath)
    failure_message = "\nHow to handle the apos inside the string inside the XPath?\nXPath = #{xpath}\n"
    assert_equal desired_result.to_s, result.to_s, failure_message
  end

  def test_xpath_03_TobiasReif
    doc = Document.new XML_STRING_02.dup
    desired_result_string = "<entry type='Book'>
    <language>english</language>
    <publisher>Addison-Wesley</publisher>
    <title>Programming Ruby. The Pragmatic Programmer's Guide</title>
    <type>Book</type>
    <year>2000</year>
  </entry>"
    desired_result_tree = Document.new desired_result_string
    xpath = "/biblio/entry[not(author)]"
    result = XPath.first(doc, xpath)
    assert_equal desired_result_string, result.to_s
  end

  def test_umlaut
    koln_iso = "K\xf6ln"
    koln_utf = "K\xc3\xb6ln"
    source_iso = "<?xml version='1.0' encoding='ISO-8859-1'?><test>#{koln_iso}</test>"
    source_utf = "<?xml version='1.0' encoding='UTF-8'?><test>#{koln_utf}</test>"

    if String.method_defined? :encode
      koln_iso.force_encoding('iso-8859-1')
      koln_utf.force_encoding('utf-8')
      source_iso.force_encoding('iso-8859-1')
      source_utf.force_encoding('utf-8')
    end

    doc = REXML::Document.new(source_iso)
    assert_equal('ISO-8859-1', doc.xml_decl.encoding)
    assert_equal(koln_utf, doc.root.text)
    doc.write(out="")
    assert_equal(source_iso, out )
    doc.xml_decl.encoding = 'UTF-8'
    doc.write(out="")
    assert_equal(source_utf, out)

    doc = Document.new <<-EOF
<?xml version="1.0" encoding="ISO-8859-1"?>
<intranet>
<position><aktuell datum="01-10-11">Technik</aktuell></position>
<hauptspalte>
<headline>Technik</headline>
Die Technik ist das R\xFCckgrat der meisten Gesch\xFCftsprozesse bei Home of the Brave. Deshalb sollen hier alle relevanten technischen Abl\xFCufe, Daten und Einrichtungen beschrieben werden, damit jeder im Bedarfsfall die n\xFCtigen Informationen, Anweisungen und Verhaltensempfehlungen nachlesen und/oder abrufen kann.
</hauptspalte>
<nebenspalte>
  <link ziel="Flash/">Flash</link><umbruch/>
  N\xFCtzliches von Flashern f\xFCr Flasher.<umbruch/>
  <link neu="ja" ziel="Cvs/">CVS-FAQ</link><umbruch/>
  FAQ zur Benutzung von CVS bei HOB
</nebenspalte>
</intranet>
EOF
    tn = XPath.first(doc, "//nebenspalte/text()[2]")
    expected_iso = "N\xFCtzliches von Flashern f\xFCr Flasher."
    expected_utf = expected_iso.unpack('C*').pack('U*')
    expected_iso.force_encoding(::Encoding::ISO_8859_1)
    expected_utf.force_encoding(::Encoding::UTF_8)
    assert_equal(expected_utf, tn.to_s.strip)
    f = REXML::Formatters::Default.new
    f.write( tn, Output.new(o = "", "ISO-8859-1") )
    assert_equal(expected_iso, o.strip)

    doc = Document.new File.new(fixture_path('xmlfile-bug.xml'))
    tn = XPath.first(doc, "//nebenspalte/text()[2]")
    assert_equal(expected_utf, tn.to_s.strip)
    f.write( tn, Output.new(o = "", "ISO-8859-1") )
    assert_equal(expected_iso, o.strip)
  end

  def test_element_cloning_namespace_Chris
    aDoc = REXML::Document.new '<h1 tpl:content="title" xmlns:tpl="1">Dummy title</h1>'

    anElement = anElement = aDoc.elements[1]
    elementAttrPrefix = anElement.attributes.get_attribute('content').prefix

    aClone = anElement.clone
    cloneAttrPrefix = aClone.attributes.get_attribute('content').prefix

    assert_equal( elementAttrPrefix , cloneAttrPrefix )
  end

  def test_namespaces_in_attlist_tobias
    in_string = File.open(fixture_path('foo.xml'), 'r') do |file|
       file.read
    end

    doc = Document.new in_string

    assert_nil XPath.first(doc,'//leg')
    assert_equal 'http://www.foo.com/human', doc.root.elements[1].namespace
    assert_equal 'human leg',
      XPath.first(doc, '//x:leg/text()', {'x'=>'http://www.foo.com/human'}).to_s
  end

  #  Alun ap Rhisiart
  def test_less_than_in_element_content
    source = File.new(fixture_path('ProductionSupport.xml'))
    h = Hash.new
    doc = REXML::Document.new source
    doc.elements.each("//CommonError") { |el|
      h[el.elements['Key'].text] = 'okay'
    }
    assert(h.include?('MotorInsuranceContract(Object)>>#error:'))
  end

  # XPaths provided by Thomas Sawyer
  def test_various_xpath
    #@doc = REXML::Document.new('<r a="1"><p><c b="2"/></p></r>')
    doc = REXML::Document.new('<r a="1"><p><c b="2">3</c></p></r>')

    [['/r', REXML::Element],
     ['/r/p/c', REXML::Element],
     ['/r/attribute::a', Attribute],
     ['/r/@a', Attribute],
     ['/r/attribute::*', Attribute],
     ['/r/@*', Attribute],
     ['/r/p/c/attribute::b', Attribute],
     ['/r/p/c/@b', Attribute],
     ['/r/p/c/attribute::*', Attribute],
     ['/r/p/c/@*', Attribute],
     ['//c/attribute::b', Attribute],
     ['//c/@b', Attribute],
     ['//c/attribute::*', Attribute],
     ['//c/@*', Attribute],
     ['.//node()', REXML::Node ],
     ['.//node()[@a]', REXML::Element ],
     ['.//node()[@a="1"]', REXML::Element ],
     ['.//node()[@b]', REXML::Element ], # no show, why?
     ['.//node()[@b="2"]', REXML::Element ]
    ].each do |xpath,kind|
      begin
        REXML::XPath.each( doc, xpath ) do |what|
          assert_kind_of( kind, what, "\n\nWrong type (#{what.class}) returned for #{xpath} (expected #{kind.name})\n\n" )
        end
      rescue Exception
        puts "PATH WAS: #{xpath}"
        raise
      end
    end

    [
     ['/r', 'attribute::a', Attribute ],
     ['/r', '@a', Attribute ],
     ['/r', 'attribute::*', Attribute ],
     ['/r', '@*', Attribute ],
     ['/r/p/c', 'attribute::b', Attribute ],
     ['/r/p/c', '@b', Attribute ],
     ['/r/p/c', 'attribute::*', Attribute ],
     ['/r/p/c', '@*', Attribute ]
    ].each do |nodepath, xpath, kind|
      begin
        context = REXML::XPath.first(doc, nodepath)
        REXML::XPath.each( context, xpath ) do |what|
          assert_kind_of kind, what, "Wrong type (#{what.class}) returned for #{xpath} (expected #{kind.name})\n"
        end
      rescue Exception
        puts "PATH WAS: #{xpath}"
        raise
      end
    end
  end

  def test_entities_Holden_Glova
    document = <<-EOL
    <?xml version="1.0" encoding="UTF-8"?>
    <!DOCTYPE rubynet [
    <!ENTITY rbconfig.MAJOR "1">
    <!ENTITY rbconfig.MINOR "7">
    <!ENTITY rbconfig.TEENY "2">
    <!ENTITY rbconfig.ruby_version "&rbconfig.MAJOR;.&rbconfig.MINOR;">
    <!ENTITY rbconfig.arch "i386-freebsd5">
    <!ENTITY rbconfig.prefix "/usr/local">
    <!ENTITY rbconfig.libdir "&rbconfig.prefix;/lib">
    <!ENTITY rbconfig.includedir "&rbconfig.prefix;/include">
    <!ENTITY rbconfig.sitedir "&rbconfig.prefix;/lib/ruby/site_ruby">
    <!ENTITY rbconfig.sitelibdir "&rbconfig.sitedir;/&rbconfig.ruby_version;">
    <!ENTITY rbconfig.sitearchdir "&rbconfig.sitelibdir;/&rbconfig.arch;">
    ]>
    <rubynet>
      <pkg version="version1.0">
        <files>
          <file>
            <filename>uga.rb</filename>
            <mode>0444</mode>
            <path>&rbconfig.libdir;/rexml</path>
            <content encoding="xml">... the file here</content>
          </file>
          <file>
            <filename>booga.h</filename>
            <mode>0444</mode>
            <path>&rbconfig.includedir;</path>
            <content encoding="xml">... the file here</content>
          </file>
          <file>
            <filename>foo.so</filename>
            <mode>0555</mode>
            <path>&rbconfig.sitearchdir;/rexml</path>
            <content encoding="mime64">Li4uIHRoZSBmaWxlIGhlcmU=\n</content>
          </file>
        </files>
      </pkg>
    </rubynet>
    EOL

    file_xpath = '/rubynet/pkg/files/file'

    root = REXML::Document.new(document)

    root.elements.each(file_xpath) do |metadata|
      text = metadata.elements['path'].get_text.value
      assert text !~ /&rbconfig/, "'#{text}' failed"
    end

    #Error occurred in test_package_file_opens(TC_PackageInstall):
    # ArgumentError:
    #illegal access mode &rbconfig.prefix;/lib/rexml
    #
    #[synack@Evergreen] src $ ruby --version
    #ruby 1.6.7 (2002-03-01) [i686-linux-gnu]
    #
    #It looks like it expanded the first entity, but didn't reparse it for more
    #entities. possible bug - or have I mucked this up?
  end

  def test_whitespace_after_xml_decl
    d = Document.new <<EOL
<?xml version='1.0'?>
  <blo>
    <wak>
    </wak>
</blo>
EOL
  end

  def test_external_entity
    xp = '//channel/title'

    %w{working.rss broken.rss}.each do |path|
      File.open(File.join(fixture_path(path))) do |file|
        doc = REXML::Document.new file.readlines.join('')

        # check to make sure everything is kosher
        assert_equal( doc.root.class, REXML::Element )
        assert_equal( doc.root.elements.class, REXML::Elements )

        # get the title of the feed
        assert( doc.root.elements[xp].kind_of?( REXML::Element ) )
      end
    end
  end

  def test_maintain_dtd
    src = %q{<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE ivattacks SYSTEM "../../ivacm.dtd" [
<!ENTITY % extern-packages SYSTEM "../../ivpackages.dtd">
<!ENTITY % extern-packages SYSTEM "../../common-declarations.dtd">
%extern-packages;
%extern-common;
]>}
    doc = Document.new( src )
    doc.write( out="" )
    src = src.tr('"', "'")
    out = out.tr('"', "'")
    assert_equal( src, out )
  end

  def test_text_nodes_nomatch
    source = "<root><child>test</child></root>"
    d = REXML::Document.new( source )
    r = REXML::XPath.match( d, %q{/root/child[text()="no-test"]} )
    assert_equal( 0, r.size )
  end

  def test_raw_Terje_Elde
    f = REXML::Formatters::Default.new
    txt = 'abc&#248;def'
    a = Text.new( txt,false,nil,true )
    f.write(a,out="")
    assert_equal( txt, out )

    txt = '<sean><russell>abc&#248;def</russell></sean>'
    a = Document.new( txt, { :raw => ["russell"] } )
    f.write(a,out="")
    assert_equal( txt, out )
  end

  def test_indenting_error
    a=Element.new("test1")
    b=Element.new("test2")
    c=Element.new("test3")
    b << c
    a << b

    REXML::Formatters::Pretty.new.write(a,s="")
  end

  def test_pos
    require 'tempfile'
    testfile = Tempfile.new("tidal")
    testdata = %Q{<calibration>
<section name="parameters">
<param name="barpress">760</param>
<param name="hertz">50</param>
</section>
</calibration>
}

    testfile.puts testdata
    testfile.rewind
    assert_nothing_raised do
      d = REXML::Document.new(testfile)
    end
    testfile.close(true)
  end

  def test_deep_clone
    a = Document.new( '<?xml version="1.0"?><!DOCTYPE html PUBLIC
    "-//W3C//DTD
    XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"><html
    xmlns="http:///www.w3.org/1999/xhtml"></html>' )
    b = a.deep_clone
    assert_equal a.to_s, b.to_s
  end

  def test_double_escaping
    data = '<title>AT&amp;T</title>'
    xml = "<description><![CDATA[#{data}]]></description>"

    doc = REXML::Document.new(xml)
    description = doc.find {|e| e.name=="description"}
    assert_equal data, description.text
  end

  def test_ticket_12
    cfg = "<element><anotherelement><child1>a</child1><child2>b</child2></anotherelement></element>"

    config = REXML::Document.new( cfg )

    assert_equal( "a", config.elements[ "//child1" ].text )
  end

=begin
 # This is a silly test, and is low priority
 def test_namespace_serialization_tobi_reif
   doc = Document.new '<doc xmlns:b="http://www.foo.foo">
 <b:p/>
</doc>'
   ns = 'http://www.foo.foo'
   ns_declaration={'f'=>ns}
   returned = XPath.match(doc,'//f:p',ns_declaration)
   # passes:
   assert( (returned[0].namespace==ns), 'namespace should be '+ns)
   serialized = returned.to_s
   serialized_and_parsed = Document.new(serialized)
   puts 'serialized: '+serialized
     # ... currently brings <b:p/>
     # prefix b is undeclared (!)
   assert( (serialized_and_parsed.namespace==ns),
     'namespace should still be '+ns.inspect+
     ' and not '+serialized_and_parsed.namespace.inspect)
   # ... currently results in a failure:
   # 'namespace should still be "http://www.foo.foo" and not ""'
 end
=end
end
