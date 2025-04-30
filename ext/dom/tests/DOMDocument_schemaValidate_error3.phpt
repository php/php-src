--TEST--
DomDocument::schemaValidate() - empty string for schema file name
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2009 NYPHP
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(__DIR__."/book.xml");

try {
    $doc->schemaValidate('');
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
DOMDocument::schemaValidate(): Argument #1 ($filename) must not be empty
