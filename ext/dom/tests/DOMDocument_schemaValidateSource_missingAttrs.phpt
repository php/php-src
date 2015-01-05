--TEST--
DomDocument::schemaValidateSource() - Don't add missing attribute default values from schema
--CREDITS--
Chris Wright <info@daverandom.com>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument;

$doc->load(dirname(__FILE__)."/book-attr.xml");

$xsd = file_get_contents(dirname(__FILE__)."/book.xsd");

$doc->schemaValidateSource($xsd);

foreach ($doc->getElementsByTagName('book') as $book) {
    var_dump($book->getAttribute('is-hardback'));
}

?>
--EXPECT--
string(0) ""
string(4) "true"
