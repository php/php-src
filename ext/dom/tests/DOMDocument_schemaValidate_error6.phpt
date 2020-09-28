--TEST--
DomDocument::schemaValidate() - invalid path to schema
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(__DIR__."/book.xml");

try {
    $doc->schemaValidate("/path/with/\0/byte");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    $doc->schemaValidate(str_repeat(" ", PHP_MAXPATHLEN + 1));
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
DOMDocument::schemaValidate(): Argument #1 ($filename) must not contain any null bytes
DOMDocument::schemaValidate(): Argument #1 ($filename) is not a valid path
