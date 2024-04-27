--TEST--
DomDocument::schemaValidateSource() - string that is not a schema
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2009 NYPHP
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(__DIR__."/book.xml");

$result = $doc->schemaValidateSource('string that is not a schema');
var_dump($result);

?>
--EXPECTF--
Warning: DOMDocument::schemaValidateSource(): Entity: line 1: parser error : Start tag expected, '<' not found in %s on line %d

Warning: DOMDocument::schemaValidateSource(): string that is not a schema in %s on line %d

Warning: DOMDocument::schemaValidateSource(): ^ in %s on line %d

Warning: DOMDocument::schemaValidateSource(): Failed to parse the XML resource 'in_memory_buffer'. in %s on line %d

Warning: DOMDocument::schemaValidateSource(): Invalid Schema in %s on line %d
bool(false)
