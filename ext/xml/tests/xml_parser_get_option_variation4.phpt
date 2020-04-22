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

$xmlParser = xml_parser_create();

var_dump(xml_parser_get_option ($xmlParser, XML_OPTION_SKIP_WHITE));

?>
--EXPECTF--
Warning: xml_parser_get_option(): Unknown option in %s on line %d
bool(false)
