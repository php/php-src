--TEST--
xml_parser_get_option() with XML_OPTION_SKIP_TAGSTART and XML_OPTION_SKIP_WHITE
--SKIPIF--
<?php
if (!extension_loaded('xml')) die('skip xml extension not available');
?>
--FILE--
<?php
$parser = xml_parser_create();
echo "defaults:\n";
var_dump(xml_parser_get_option($parser, XML_OPTION_SKIP_TAGSTART));
var_dump(xml_parser_get_option($parser, XML_OPTION_SKIP_WHITE));
echo "setting:\n";
var_dump(xml_parser_set_option($parser, XML_OPTION_SKIP_TAGSTART, 7));
var_dump(xml_parser_set_option($parser, XML_OPTION_SKIP_WHITE, 1));
echo "getting:\n";
var_dump(xml_parser_get_option($parser, XML_OPTION_SKIP_TAGSTART));
var_dump(xml_parser_get_option($parser, XML_OPTION_SKIP_WHITE));
?>
--EXPECT--
defaults:
int(0)
int(0)
setting:
bool(true)
bool(true)
getting:
int(7)
int(1)
