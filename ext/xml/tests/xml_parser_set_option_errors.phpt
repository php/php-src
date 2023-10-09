--TEST--
xml_parser_set_option(): Setting options to invalid values
--EXTENSIONS--
xml
--FILE--
<?php

$xmlParser = xml_parser_create();

echo "Case folding\n";
try {
    xml_parser_set_option($xmlParser, XML_OPTION_CASE_FOLDING, []);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    xml_parser_set_option($xmlParser, XML_OPTION_CASE_FOLDING, new stdClass());
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Skip Whitespace\n";
try {
    xml_parser_set_option($xmlParser, XML_OPTION_SKIP_WHITE, []);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    xml_parser_set_option($xmlParser, XML_OPTION_SKIP_WHITE, new stdClass());
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Tag Start\n";
xml_parser_set_option($xmlParser, XML_OPTION_SKIP_TAGSTART, -5);

xml_parser_set_option($xmlParser, XML_OPTION_SKIP_TAGSTART, []);

xml_parser_set_option($xmlParser, XML_OPTION_SKIP_TAGSTART, new stdClass());

echo "Encodings\n";
try {
    xml_parser_set_option($xmlParser, XML_OPTION_TARGET_ENCODING, 'Invalid Encoding');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    xml_parser_set_option($xmlParser, XML_OPTION_TARGET_ENCODING, []);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    xml_parser_set_option($xmlParser, XML_OPTION_TARGET_ENCODING, new stdClass());
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage() . "\n";
}

?>
--EXPECTF--
Case folding

Warning: xml_parser_set_option(): Argument #3 ($value) must be of type string|int|bool, array given in %s on line %d

Warning: xml_parser_set_option(): Argument #3 ($value) must be of type string|int|bool, stdClass given in %s on line %d
Skip Whitespace

Warning: xml_parser_set_option(): Argument #3 ($value) must be of type string|int|bool, array given in %s on line %d

Warning: xml_parser_set_option(): Argument #3 ($value) must be of type string|int|bool, stdClass given in %s on line %d
Tag Start

Warning: xml_parser_set_option(): Argument #3 ($value) must be between 0 and 2147483647 for option XML_OPTION_SKIP_TAGSTART in %s on line %d

Warning: xml_parser_set_option(): Argument #3 ($value) must be of type string|int|bool, array given in %s on line %d

Warning: xml_parser_set_option(): Argument #3 ($value) must be of type string|int|bool, stdClass given in %s on line %d

Warning: Object of class stdClass could not be converted to int in %s on line %d
Encodings
xml_parser_set_option(): Argument #3 ($value) is not a supported target encoding

Warning: xml_parser_set_option(): Argument #3 ($value) must be of type string|int|bool, array given in %s on line %d

Warning: Array to string conversion in %s on line %d
xml_parser_set_option(): Argument #3 ($value) is not a supported target encoding

Warning: xml_parser_set_option(): Argument #3 ($value) must be of type string|int|bool, stdClass given in %s on line %d
Error: Object of class stdClass could not be converted to string
