--TEST--
DomDocument::schemaValidate() - non-existent schema file
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2009 NYPHP
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(__DIR__."/book.xml");

$result = $doc->schemaValidate(__DIR__."/non-existent-file");
var_dump($result);

?>
--EXPECTF--
Warning: DOM\Document::schemaValidate(): I/O warning : failed to load external entity "/home/niels/php-src/ext/dom/tests/non-existent-file" in %s on line %d

Warning: DOM\Document::schemaValidate(): Failed to locate the main schema resource at '/home/niels/php-src/ext/dom/tests/non-existent-file'. in %s on line %d

Warning: DOM\Document::schemaValidate(): Invalid Schema in %s on line %d
bool(false)
