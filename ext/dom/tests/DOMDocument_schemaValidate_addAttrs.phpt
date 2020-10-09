--TEST--
DomDocument::schemaValidate() - Add missing attribute default values from schema
--CREDITS--
Chris Wright <info@daverandom.com>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(__DIR__."/book-attr.xml");

$doc->schemaValidate(__DIR__."/book.xsd", LIBXML_SCHEMA_CREATE);

foreach ($doc->getElementsByTagName('book') as $book) {
    var_dump($book->getAttribute('is-hardback'));
}

?>
--EXPECT--
string(5) "false"
string(4) "true"
