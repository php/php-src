--TEST--
DomDocument::schemaValidate() - Don't add missing attribute default values from schema
--CREDITS--
Chris Wright <info@daverandom.com>
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(__DIR__."/book-attr.xml");

$doc->schemaValidate(__DIR__."/book.xsd");

foreach ($doc->getElementsByTagName('book') as $book) {
    var_dump($book->getAttribute('is-hardback'));
}

?>
--EXPECT--
string(0) ""
string(4) "true"
