--TEST--
DomDocument::schemaValidate() - pass no parameters
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2009 NYPHP
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(dirname(__FILE__)."/book.xml");

$result = $doc->schemaValidate();
var_dump($result);

?>
--EXPECTF--
Warning: DOMDocument::schemaValidate() expects at least 1 parameter, 0 given in %s.php on line %d
NULL
