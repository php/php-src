--TEST--
DomDocument::schemaValidateSource() - empty string for schema string
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
    $doc->schemaValidateSource('');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: DOMDocument::schemaValidateSource(): Argument #1 ($source) must not be empty
