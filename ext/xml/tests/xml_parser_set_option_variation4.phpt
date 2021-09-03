--TEST--
xml_parser_free - Test setting skip whitespace and invalid encoding type
--CREDITS--
Mark Niebergall <mbniebergall@gmail.com>
PHP TestFest 2017 - UPHPU
--EXTENSIONS--
xml
--FILE--
<?php

$xmlParser = xml_parser_create();

var_dump(xml_parser_set_option($xmlParser, XML_OPTION_SKIP_WHITE, 1));

try {
    xml_parser_set_option($xmlParser, XML_OPTION_TARGET_ENCODING, 'Invalid Encoding');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
bool(true)
xml_parser_set_option(): Argument #3 ($value) is not a supported target encoding
