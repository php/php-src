--TEST--
DomDocument::schemaValidate() - non-conforming schema file
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2009 NYPHP
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(__DIR__."/book.xml");

$result = $doc->schemaValidate(__DIR__."/book-non-conforming-schema.xsd");
var_dump($result);

?>
--EXPECTF--
Warning: DOMDocument::schemaValidate(): Element 'books': No matching global declaration available for the validation root. in %s on line %d
bool(false)
