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
Warning: DOMDocument::schemaValidate(): I/O warning : failed to load external entity "%snon-existent-file" in %s.php on line %d

Warning: DOMDocument::schemaValidate(): Failed to locate the main schema resource at '%s/non-existent-file'. in %s.php on line %d

Warning: DOMDocument::schemaValidate(): Invalid Schema in %s.php on line %d
bool(false)
