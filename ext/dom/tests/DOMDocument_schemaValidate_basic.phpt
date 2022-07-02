--TEST--
DomDocument::schemaValidate() - basic
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2009 NYPHP
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(__DIR__."/book.xml");

$result = $doc->schemaValidate(__DIR__."/book.xsd");
var_dump($result);

?>
--EXPECT--
bool(true)
