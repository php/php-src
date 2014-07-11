--TEST--
DomDocument::schemaValidate() - basic
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2009 NYPHP
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(dirname(__FILE__)."/book.xml");

$result = $doc->schemaValidate(dirname(__FILE__)."/book.xsd");
var_dump($result);

?>
--EXPECT--
bool(true)
