--TEST--
DomDocument::schemaValidate() - invalid path to schema
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(__DIR__."/book.xml");

try {
    $doc->schemaValidate("/path/with/\0/byte");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

var_dump($doc->schemaValidate(str_repeat(" ", PHP_MAXPATHLEN + 1)));

?>
--EXPECTF--
DOM\Document::schemaValidate(): Argument #1 ($filename) must not contain any null bytes

Warning: DOM\Document::schemaValidate(): Invalid Schema file source in %s on line %d
bool(false)
