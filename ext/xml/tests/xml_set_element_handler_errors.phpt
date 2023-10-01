--TEST--
Error conditions when setting invalid handler callables for xml_set_element_handler()
--EXTENSIONS--
xml
--FILE--
<?php
declare(strict_types=1);

function dummy() {}

echo 'Invalid $parser:', PHP_EOL;
$obj = new stdClass();
try {
    xml_set_element_handler($obj, null, null);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* Create valid parser */
$parser = xml_parser_create();

echo 'Invalid start callable type true:', PHP_EOL;
try {
    xml_set_element_handler($parser, true, null);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo 'Invalid end callable type true:', PHP_EOL;
try {
    xml_set_element_handler($parser, null, true);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo 'Invalid start callable type int:', PHP_EOL;
try {
    xml_set_element_handler($parser, 10, null);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo 'Invalid end callable type int:', PHP_EOL;
try {
    xml_set_element_handler($parser, null, 10);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo 'Invalid start callable, no object set and string not callable:', PHP_EOL;
try {
    xml_set_element_handler($parser, "nonexistent_method", null);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo 'Invalid end callable, no object set and string not callable:', PHP_EOL;
try {
    xml_set_element_handler($parser, null, "nonexistent_method");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo 'Invalid start callable, string non existent method on set object:', PHP_EOL;
xml_set_object($parser, $obj);
try {
    xml_set_element_handler($parser, "nonexistent_method", null);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo 'Invalid end callable, string non existent method on set object:', PHP_EOL;
xml_set_object($parser, $obj);
try {
    xml_set_element_handler($parser, null, "nonexistent_method");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Invalid $parser:
TypeError: xml_set_element_handler(): Argument #1 ($parser) must be of type XMLParser, stdClass given
Invalid start callable type true:
TypeError: xml_set_element_handler(): Argument #2 ($start_handler) must be of type callable|string|null|false
Invalid end callable type true:
TypeError: xml_set_element_handler(): Argument #3 ($end_handler) must be of type callable|string|null|false
Invalid start callable type int:
TypeError: xml_set_element_handler(): Argument #2 ($start_handler) must be of type callable|string|null|false
Invalid end callable type int:
TypeError: xml_set_element_handler(): Argument #3 ($end_handler) must be of type callable|string|null|false
Invalid start callable, no object set and string not callable:
ValueError: xml_set_element_handler(): Argument #2 ($start_handler) an object must be set via xml_set_object() to be able to lookup method
Invalid end callable, no object set and string not callable:
ValueError: xml_set_element_handler(): Argument #3 ($end_handler) an object must be set via xml_set_object() to be able to lookup method
Invalid start callable, string non existent method on set object:
ValueError: xml_set_element_handler(): Argument #2 ($start_handler) method stdClass::nonexistent_method() does not exist
Invalid end callable, string non existent method on set object:
ValueError: xml_set_element_handler(): Argument #3 ($end_handler) method stdClass::nonexistent_method() does not exist
