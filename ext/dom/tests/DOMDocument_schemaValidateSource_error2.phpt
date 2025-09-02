--TEST--
DomDocument::schemaValidateSource() - non-conforming schema
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2009 NYPHP
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(__DIR__."/book.xml");

$xsd = file_get_contents(__DIR__."/book-non-conforming-schema.xsd");

$result = $doc->schemaValidateSource($xsd);
var_dump($result);

?>
--EXPECTF--
Warning: DOMDocument::schemaValidateSource(): Element 'books': No matching global declaration available for the validation root. in %s on line %d
bool(false)
