--TEST--
Request #68325 (XML_OPTION_PARSE_HUGE cannot be set for xml_parser_create - setting during parsing)
--EXTENSIONS--
xml
--SKIPIF--
<?php
if (!defined("LIBXML_VERSION")) die('skip this is a libxml2 test');
?>
--FILE--
<?php

$parser = xml_parser_create();
xml_set_element_handler($parser, function($parser, $data) {
    xml_parser_set_option($parser, XML_OPTION_PARSE_HUGE, true);
}, function($parser, $data) {
});
xml_parse($parser, "<foo/>", true);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot change option XML_OPTION_PARSE_HUGE while parsing in %s:%d
Stack trace:
#0 %s(%d): xml_parser_set_option(Object(XMLParser), 5, true)
#1 [internal function]: {closure:%s:%d}(Object(XMLParser), 'FOO', Array)
#2 %s(%d): xml_parse(Object(XMLParser), '<foo/>', true)
#3 {main}
  thrown in %s on line %d
