--TEST--
Bug #30875 (xml_parse_into_struct() does not resolve entities)
--EXTENSIONS--
xml
--FILE--
<?php

$xml = <<<XML
<!DOCTYPE dtd [
    <!ENTITY ref "ent">
]>
<elt att="&ref;">a&ref;</elt>
XML;

$parser = xml_parser_create();
xml_parse_into_struct($parser, $xml, $vals);
xml_parser_free($parser);
var_dump($vals);
?>
--EXPECT--
array(1) {
  [0]=>
  array(5) {
    ["tag"]=>
    string(3) "ELT"
    ["type"]=>
    string(8) "complete"
    ["level"]=>
    int(1)
    ["attributes"]=>
    array(1) {
      ["ATT"]=>
      string(3) "ent"
    }
    ["value"]=>
    string(4) "aent"
  }
}
