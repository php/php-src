--TEST--
Dom\XMLSchema: construction and argument errors
--EXTENSIONS--
dom
--FILE--
<?php
$schema = Dom\XMLSchema::createFromFile(__DIR__ . '/../../book.xsd');
$cases = [
    fn() => new Dom\XMLSchema(),
    fn() => clone $schema,
    fn() => serialize($schema),
    fn() => Dom\XMLSchema::createFromString(''),
    fn() => Dom\XMLSchema::createFromFile("a\0b"),
    fn() => @Dom\XMLSchema::createFromString('<not-a-schema/>'),
    fn() => @Dom\XMLSchema::createFromFile(__DIR__ . '/does-not-exist.xsd'),
    fn() => $schema->validate(new stdClass()),
    fn() => $schema->validate(Dom\HTMLDocument::createEmpty()),
];
foreach ($cases as $case) {
    try {
        $case();
        echo "no exception\n";
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
Error: Call to private Dom\XMLSchema::__construct() from global scope
Error: Trying to clone an uncloneable object of class Dom\XMLSchema
Exception: Serialization of 'Dom\XMLSchema' is not allowed
ValueError: Dom\XMLSchema::createFromString(): Argument #1 ($source) must not be empty
ValueError: Dom\XMLSchema::createFromFile(): Argument #1 ($path) must not contain any null bytes
Exception: Invalid Schema
Exception: Invalid Schema
TypeError: Dom\XMLSchema::validate(): Argument #1 ($document) must be of type Dom\XMLDocument|DOMDocument, stdClass given
TypeError: Dom\XMLSchema::validate(): Argument #1 ($document) must be of type Dom\XMLDocument|DOMDocument, Dom\HTMLDocument given
