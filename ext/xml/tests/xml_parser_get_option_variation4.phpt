--TEST--
xml_parser_get_option() - Test parameter not set
--EXTENSIONS--
xml
--FILE--
<?php

$xmlParser = xml_parser_create();

try {
    xml_parser_get_option ($xmlParser, 42);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
xml_parser_get_option(): Argument #2 ($option) must be a XML_OPTION_* constant
