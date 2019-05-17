--TEST--
DomDocument::schemaValidateSource() - basic
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2009 NYPHP
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(__DIR__."/book.xml");

$xsd = file_get_contents(__DIR__."/book.xsd");

$result = $doc->schemaValidateSource($xsd);
var_dump($result);

?>
--EXPECT--
bool(true)
