--TEST--
GH-14124 (Segmentation fault on unknown address 0x0001ffff8041 with XML extension under certain memory limit)
--EXTENSIONS--
xml
--INI--
memory_limit=33M
--SKIPIF--
<?php
if (!defined("LIBXML_VERSION")) die('skip this is a libxml2 test');
if (getenv('SKIP_ASAN')) die('xleak libxml does not use the request allocator');
?>
--FILE--
<?php
function createParser(bool $huge) {
    $parser = xml_parser_create();
    xml_parser_set_option($parser, XML_OPTION_PARSE_HUGE, $huge);
    return $parser;
}

$long_text = str_repeat("A", 1000 * 1000 * 5 /* 5 MB */);
$long_xml_head = "<?xml version=\"1.0\"?><container><$long_text/><$long_text/><second>foo</second>";
$long_xml_tail = "</container>";
$parser = createParser(true);
xml_parse_into_struct($parser, $long_xml_head . $long_xml_tail, $values, $index);
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %s in %s on line %d
