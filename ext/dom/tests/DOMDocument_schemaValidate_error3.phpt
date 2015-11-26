--TEST--
DomDocument::schemaValidate() - empty string for schema file name
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2009 NYPHP
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(dirname(__FILE__)."/book.xml");

$result = $doc->schemaValidate('');
var_dump($result);

?>
--EXPECTF--
Warning: DOMDocument::schemaValidate(): Invalid Schema source in %s.php on line %d
bool(false)
