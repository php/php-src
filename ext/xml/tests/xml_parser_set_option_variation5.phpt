--TEST--
xml_parser_set_option() - Test invalid parameter
--EXTENSIONS--
xml
--FILE--
<?php

$xmlParser = xml_parser_create();

try {
    xml_parser_set_option($xmlParser, 42, 1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
xml_parser_set_option(): Argument #2 ($option) must be a XML_OPTION_* constant
