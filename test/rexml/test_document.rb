# -*- coding: utf-8 -*-

require "rexml/document"
require "test/unit"

class REXML::TestDocument < Test::Unit::TestCase
  def test_version_attributes_to_s
    doc = REXML::Document.new(<<-eoxml)
      <?xml version="1.0" encoding="UTF-8" standalone="no"?>
      <svg  id="svg2"
            xmlns:sodipodi="foo"
            xmlns:inkscape="bar"
            sodipodi:version="0.32"
            inkscape:version="0.44.1"
      >
      </svg>
    eoxml

    string = doc.to_s
    assert_match('xmlns:sodipodi', string)
    assert_match('xmlns:inkscape', string)
    assert_match('sodipodi:version', string)
    assert_match('inkscape:version', string)
  end

  def test_new
    doc = REXML::Document.new(<<EOF)
<?xml version="1.0" encoding="UTF-8"?>
<message>Hello world!</message>
EOF
    assert_equal("Hello world!", doc.root.children.first.value)
  end

  XML_WITH_NESTED_ENTITY = <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE member [
  <!ENTITY a "&b;&b;&b;&b;&b;&b;&b;&b;&b;&b;">
  <!ENTITY b "&c;&c;&c;&c;&c;&c;&c;&c;&c;&c;">
  <!ENTITY c "&d;&d;&d;&d;&d;&d;&d;&d;&d;&d;">
  <!ENTITY d "&e;&e;&e;&e;&e;&e;&e;&e;&e;&e;">
  <!ENTITY e "&f;&f;&f;&f;&f;&f;&f;&f;&f;&f;">
  <!ENTITY f "&g;&g;&g;&g;&g;&g;&g;&g;&g;&g;">
  <!ENTITY g "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx">
]>
<member>
&a;
</member>
EOF

  XML_WITH_4_ENTITY_EXPANSION = <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE member [
  <!ENTITY a "a">
  <!ENTITY a2 "&a; &a;">
]>
<member>
&a;
&a2;
&lt;
</member>
EOF

  def test_entity_expansion_limit
    doc = REXML::Document.new(XML_WITH_NESTED_ENTITY)
    assert_raise(RuntimeError) do
      doc.root.children.first.value
    end
    REXML::Document.entity_expansion_limit = 100
    assert_equal(100, REXML::Document.entity_expansion_limit)
    doc = REXML::Document.new(XML_WITH_NESTED_ENTITY)
    assert_raise(RuntimeError) do
      doc.root.children.first.value
    end
    assert_equal(101, doc.entity_expansion_count)

    REXML::Document.entity_expansion_limit = 4
    doc = REXML::Document.new(XML_WITH_4_ENTITY_EXPANSION)
    assert_equal("\na\na a\n<\n", doc.root.children.first.value)
    REXML::Document.entity_expansion_limit = 3
    doc = REXML::Document.new(XML_WITH_4_ENTITY_EXPANSION)
    assert_raise(RuntimeError) do
      doc.root.children.first.value
    end
  ensure
    REXML::Document.entity_expansion_limit = 10000
  end

  def test_tag_in_cdata_with_not_ascii_only_but_ascii8bit_encoding_source
    tag = "<b>...</b>"
    message = "こんにちは、世界！" # Hello world! in Japanese
    xml = <<EOX
<?xml version="1.0" encoding="UTF-8"?>
<message><![CDATA[#{tag}#{message}]]></message>
EOX
    xml.force_encoding(Encoding::ASCII_8BIT)
    doc = REXML::Document.new(xml)
    assert_equal("#{tag}#{message}", doc.root.children.first.value)
  end

  def test_xml_declaration_standalone
    bug2539 = '[ruby-core:27345]'
    doc = REXML::Document.new('<?xml version="1.0" standalone="no" ?>')
    assert_equal('no', doc.stand_alone?, bug2539)
    doc = REXML::Document.new('<?xml version="1.0" standalone= "no" ?>')
    assert_equal('no', doc.stand_alone?, bug2539)
    doc = REXML::Document.new('<?xml version="1.0" standalone=  "no" ?>')
    assert_equal('no', doc.stand_alone?, bug2539)
  end
end
