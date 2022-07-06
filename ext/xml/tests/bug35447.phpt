--TEST--
Bug #35447 (xml_parse_into_struct() chokes on the UTF-8 BOM)
--SKIPIF--
<?php
require_once("skipif.inc");
if (! @xml_parser_create_ns('ISO-8859-1')) { die("skip xml_parser_create_ns is not supported on this platform");}
?>
--FILE--
<?php
$data = <<<END_OF_XML
\xEF\xBB\xBF<?xml version="1.0" encoding="utf-8"?\x3e
<!DOCTYPE bundle [
    <!ELEMENT bundle (resource)+>
    <!ELEMENT resource (#PCDATA)>
    <!ATTLIST resource
              key CDATA #REQUIRED
              type (literal|pattern|sub) "literal"
              >
]>
<resource key="rSeeYou">A bient&amp;244;t</resource>
END_OF_XML;

$parser = xml_parser_create_ns('UTF-8');
xml_parser_set_option($parser,XML_OPTION_CASE_FOLDING,0);
$result = xml_parse_into_struct($parser, $data, $vals, $index);
xml_parser_free($parser);
var_dump($vals);
?>
--EXPECT--
array(1) {
  [0]=>
  array(5) {
    ["tag"]=>
    string(8) "resource"
    ["type"]=>
    string(8) "complete"
    ["level"]=>
    int(1)
    ["attributes"]=>
    array(2) {
      ["key"]=>
      string(7) "rSeeYou"
      ["type"]=>
      string(7) "literal"
    }
    ["value"]=>
    string(13) "A bient&244;t"
  }
}
