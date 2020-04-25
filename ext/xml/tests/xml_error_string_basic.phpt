--TEST--
xml_parser_get_option() - Test parameter not set
--SKIPIF--
<?php
if (!extension_loaded('xml')) {
    exit('Skip - XML extension not loaded');
}
?>
--FILE--
<?php

$xml = '<?xml?>';

$xml_parser = xml_parser_create();

if (!xml_parse($xml_parser, $xml, true)) {
    var_dump(xml_get_error_code($xml_parser));
    var_dump(xml_error_string(xml_get_error_code($xml_parser)));
}
?>
--EXPECTF--
int(64)
string(17) "Reserved XML Name"
