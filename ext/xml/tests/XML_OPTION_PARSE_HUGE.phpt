--TEST--
Request #68325 (XML_OPTION_PARSE_HUGE cannot be set for xml_parser_create)
--EXTENSIONS--
xml
--SKIPIF--
<?php
if (!defined("LIBXML_VERSION")) die('skip this is a libxml2 test');
?>
--FILE--
<?php

function logName(string $str) {
    if (strlen($str) > 20) {
        echo substr($str, 0, 20) . "...\n";
    } else {
        echo $str . "\n";
    }
}

function createParser(bool $huge) {
    $parser = xml_parser_create();
    echo "old option value: "; var_dump(xml_parser_get_option($parser, XML_OPTION_PARSE_HUGE));
    xml_parser_set_option($parser, XML_OPTION_PARSE_HUGE, $huge);
    echo "new option value: "; var_dump(xml_parser_get_option($parser, XML_OPTION_PARSE_HUGE));
    xml_set_element_handler($parser, function($parser, $data) {
        echo "open: ";
        logName($data);
    }, function($parser, $data) {
    });
    return $parser;
}

// Construct XML that is too large to parse without XML_OPTION_PARSE_HUGE
$long_text = str_repeat("A", 1000 * 1000 * 5 /* 5 MB */);
$long_xml_head = "<?xml version=\"1.0\"?><container><$long_text/><$long_text/><second>foo</second>";
$long_xml_tail = "</container>";

echo "--- Parse using xml_parse (failure) ---\n";
$parser = createParser(false);
$ret = xml_parse($parser, $long_xml_head, true);
echo "ret = $ret (", xml_error_string(xml_get_error_code($parser)), ")\n";

echo "--- Parse using xml_parse (success) ---\n";
$parser = createParser(true);
$ret = xml_parse($parser, $long_xml_head, false);
echo "ret = $ret (", xml_error_string(xml_get_error_code($parser)), ")\n";
$ret = xml_parse($parser, $long_xml_tail, true);
echo "ret = $ret (", xml_error_string(xml_get_error_code($parser)), ")\n";

echo "--- Parse using xml_parse_into_struct (failure) ---\n";
$parser = createParser(false);
$ret = xml_parse_into_struct($parser, $long_xml_head, $values, $index);
echo "ret = $ret (", xml_error_string(xml_get_error_code($parser)), ")\n";

echo "--- Parse using xml_parse_into_struct (success) ---\n";
$parser = createParser(true);
$ret = xml_parse_into_struct($parser, $long_xml_head . $long_xml_tail, $values, $index);
var_dump(count($values), count($index)); // Not printing out the raw array because the long string will be contained in them as key
echo "ret = $ret (", xml_error_string(xml_get_error_code($parser)), ")\n";

?>
--EXPECT--
--- Parse using xml_parse (failure) ---
old option value: bool(false)
new option value: bool(false)
open: CONTAINER
ret = 0 (XML_ERR_NAME_REQUIRED)
--- Parse using xml_parse (success) ---
old option value: bool(false)
new option value: bool(true)
open: CONTAINER
open: AAAAAAAAAAAAAAAAAAAA...
open: AAAAAAAAAAAAAAAAAAAA...
open: SECOND
ret = 1 (No error)
ret = 1 (No error)
--- Parse using xml_parse_into_struct (failure) ---
old option value: bool(false)
new option value: bool(false)
open: CONTAINER
ret = 0 (XML_ERR_NAME_REQUIRED)
--- Parse using xml_parse_into_struct (success) ---
old option value: bool(false)
new option value: bool(true)
open: CONTAINER
open: AAAAAAAAAAAAAAAAAAAA...
open: AAAAAAAAAAAAAAAAAAAA...
open: SECOND
int(5)
int(3)
ret = 1 (No error)
