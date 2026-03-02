--TEST--
DomDocument::schemaValidateSource() - Add missing attribute default values from schema
--CREDITS--
Chris Wright <info@daverandom.com>
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(__DIR__."/book-attr.xml");

$xsd = file_get_contents(__DIR__."/book.xsd");

$doc->schemaValidateSource($xsd, LIBXML_SCHEMA_CREATE);

foreach ($doc->getElementsByTagName('book') as $book) {
    var_dump($book->getAttribute('is-hardback'));
}

?>
--EXPECT--
string(5) "false"
string(4) "true"
