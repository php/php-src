--TEST--
GH-14637 memory leak under memory limit
--EXTENSIONS--
xml
--INI--
memory_limit=20M
--CREDITS--
YuanchengJiang
--FILE--
<?php
function createParser(bool $huge) {
    $parser = xml_parser_create();
    xml_parser_set_option($parser, XML_OPTION_PARSE_HUGE, $huge);
    xml_set_element_handler($parser, function($parser, $data) {
    }, function($parser, $data) {
    });
    return $parser;
}
// Construct XML that is too large to parse without XML_OPTION_PARSE_HUGE
$long_text = str_repeat("A", 1000 * 1000 * 5 /* 5 MB */);
$long_xml_head = "<?xml version=\"1.0\"?><container><$long_text/><$long_text/><second>foo</second>";
$long_xml_tail = "</container>";
$parser = createParser(false);
$ret = xml_parse($parser, $long_xml_head, true);
$parser = createParser(true);
$ret = xml_parse($parser, $long_xml_head, false);
$parser = createParser(true);
$ret = xml_parse_into_struct($parser, $long_xml_head . $long_xml_tail, $values, $index);
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %s in %s on line %d
