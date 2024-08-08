--TEST--
Error conditions when setting invalid handler callables
--EXTENSIONS--
xml
--FILE--
<?php
declare(strict_types=1);

/* Use xml_set_processing_instruction_handler() for generic implementation */
echo 'Invalid $parser:', PHP_EOL;
$obj = new stdClass();
try {
    xml_set_processing_instruction_handler($obj, null);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* Create valid parser */
$parser = xml_parser_create();

echo 'Invalid callable type true:', PHP_EOL;
try {
    xml_set_processing_instruction_handler($parser, true);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo 'Invalid callable type int:', PHP_EOL;
try {
    xml_set_processing_instruction_handler($parser, 10);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo 'String not callable and no object set:', PHP_EOL;
try {
    xml_set_processing_instruction_handler($parser, "nonexistent_method");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo 'String non existent method on set object:', PHP_EOL;
xml_set_object($parser, $obj);
try {
    xml_set_processing_instruction_handler($parser, "nonexistent_method");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Invalid $parser:
TypeError: xml_set_processing_instruction_handler(): Argument #1 ($parser) must be of type XMLParser, stdClass given
Invalid callable type true:
TypeError: xml_set_processing_instruction_handler(): Argument #2 ($handler) must be of type callable|string|null
Invalid callable type int:
TypeError: xml_set_processing_instruction_handler(): Argument #2 ($handler) must be of type callable|string|null
String not callable and no object set:

Deprecated: xml_set_processing_instruction_handler(): Passing non-callable strings is deprecated since 8.4 in %s on line %d
ValueError: xml_set_processing_instruction_handler(): Argument #2 ($handler) an object must be set via xml_set_object() to be able to lookup method
String non existent method on set object:

Deprecated: Function xml_set_object() is deprecated since 8.4, provide a proper method callable to xml_set_*_handler() functions in %s on line %d

Deprecated: xml_set_processing_instruction_handler(): Passing non-callable strings is deprecated since 8.4 in %s on line %d
ValueError: xml_set_processing_instruction_handler(): Argument #2 ($handler) method stdClass::nonexistent_method() does not exist
