--TEST--
XML_OPTION_SKIP_TAGSTART bounds
--EXTENSIONS--
xml
--FILE--
<?php
$sample = "<?xml version=\"1.0\"?><test><child/></test>";
$parser = xml_parser_create();
xml_parser_set_option($parser, XML_OPTION_SKIP_TAGSTART, 100);
$res = xml_parse_into_struct($parser,$sample,$vals,$index);
var_dump($vals);
?>
--EXPECT--
array(3) {
  [0]=>
  array(3) {
    ["tag"]=>
    string(0) ""
    ["type"]=>
    string(4) "open"
    ["level"]=>
    int(1)
  }
  [1]=>
  array(3) {
    ["tag"]=>
    string(0) ""
    ["type"]=>
    string(8) "complete"
    ["level"]=>
    int(2)
  }
  [2]=>
  array(3) {
    ["tag"]=>
    string(0) ""
    ["type"]=>
    string(5) "close"
    ["level"]=>
    int(1)
  }
}
