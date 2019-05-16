--TEST--
xml_parser_free - Test setting skip whitespace and invalid encoding type
--CREDITS--
Mark Niebergall <mbniebergall@gmail.com>
PHP TestFest 2017 - UPHPU
--SKIPIF--
<?php
if (!extension_loaded("xml")) {
    print "skip - XML extension not loaded";
}
?>
--FILE--
<?php

$xmlParser = xml_parser_create();

var_dump(xml_parser_set_option($xmlParser, XML_OPTION_SKIP_WHITE, 1));
var_dump(xml_parser_set_option($xmlParser, XML_OPTION_TARGET_ENCODING, 'Invalid Encoding'));

?>
--EXPECTF--
bool(true)

Warning: xml_parser_set_option(): Unsupported target encoding "Invalid Encoding" in %s on line %d
bool(false)
