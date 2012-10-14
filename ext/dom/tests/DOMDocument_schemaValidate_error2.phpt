--TEST--
DomDocument::schemaValidate() - non-conforming schema file
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2009 NYPHP
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(dirname(__FILE__)."/book.xml");

$result = $doc->schemaValidate(dirname(__FILE__)."/book-non-conforming-schema.xsd");
var_dump($result);

?>
--EXPECTF--
Warning: DOMDocument::schemaValidate(): Element 'books': No matching global declaration available for the validation root. in %s.php on line %d
bool(false)
