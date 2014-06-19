--TEST--
DomDocument::schemaValidate() - non-existent schema file
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2009 NYPHP
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(dirname(__FILE__)."/book.xml");

$result = $doc->schemaValidate(dirname(__FILE__)."/non-existent-file");
var_dump($result);

?>
--EXPECTF--
Warning: DOMDocument::schemaValidate(): I/O warning : failed to load external entity "%snon-existent-file" in %s.php on line %d

Warning: DOMDocument::schemaValidate(): Failed to locate the main schema resource at '%s/non-existent-file'. in %s.php on line %d

Warning: DOMDocument::schemaValidate(): Invalid Schema in %s.php on line %d
bool(false)
