--TEST--
DomDocument::schemaValidateSource() - empty string for schema string
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2009 NYPHP
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(dirname(__FILE__)."/book.xml");

$result = $doc->schemaValidateSource('');
var_dump($result);

?>
--EXPECTF--
Warning: DOMDocument::schemaValidateSource(): Invalid Schema source in %s.php on line %d
bool(false)
