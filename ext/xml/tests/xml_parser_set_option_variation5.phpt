--TEST--
xml_parser_set_option() - Test that an invalid parameter defaults
and returns a E_WARNING-level message with "Unknown option" text
--CREDITS--
Symeon Charalabides <symeon@systasis.com> - @phpdublin
--SKIPIF--
<?php
if (!extension_loaded('xml')) {
    exit('Skip - XML extension not loaded');
}
?>
--FILE--
<?php

$xmlParser = xml_parser_create();

var_dump(xml_parser_set_option($xmlParser, 42, 1)); 

?>
--EXPECTF--
Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)
