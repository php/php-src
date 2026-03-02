--TEST--
xml_parser_get_option() with XML_OPTION_SKIP_TAGSTART and XML_OPTION_SKIP_WHITE
--EXTENSIONS--
xml
--FILE--
<?php
$parser = xml_parser_create();
echo "defaults:\n";
var_dump(xml_parser_get_option($parser, XML_OPTION_SKIP_TAGSTART));
var_dump(xml_parser_get_option($parser, XML_OPTION_SKIP_WHITE));
var_dump(xml_parser_get_option($parser, XML_OPTION_CASE_FOLDING));
echo "setting:\n";
var_dump(xml_parser_set_option($parser, XML_OPTION_SKIP_TAGSTART, 7));
var_dump(xml_parser_set_option($parser, XML_OPTION_SKIP_WHITE, 1));
var_dump(xml_parser_set_option($parser, XML_OPTION_CASE_FOLDING, false));
echo "getting:\n";
var_dump(xml_parser_get_option($parser, XML_OPTION_SKIP_TAGSTART));
var_dump(xml_parser_get_option($parser, XML_OPTION_SKIP_WHITE));
var_dump(xml_parser_get_option($parser, XML_OPTION_CASE_FOLDING));
?>
--EXPECT--
defaults:
int(0)
bool(false)
bool(true)
setting:
bool(true)
bool(true)
bool(true)
getting:
int(7)
bool(true)
bool(false)
