--TEST--
Dom\XMLSchema: compile once, validate several documents
--EXTENSIONS--
dom
--FILE--
<?php
$schema = Dom\XMLSchema::createFromFile(__DIR__ . '/../../book.xsd');
$fromString = Dom\XMLSchema::createFromString(file_get_contents(__DIR__ . '/../../book.xsd'));

$legacy = new DOMDocument();
$legacy->load(__DIR__ . '/../../book.xml');
$modern = Dom\XMLDocument::createFromFile(__DIR__ . '/../../book.xml');
$invalid = Dom\XMLDocument::createFromString('<books><book><title>T</title></book></books>');

var_dump($schema->validate($legacy));
var_dump($schema->validate($modern));
var_dump($fromString->validate($modern));

libxml_use_internal_errors(true);
var_dump($schema->validate($invalid));
var_dump(count(libxml_get_errors()) > 0);
libxml_clear_errors();
var_dump($schema->validate($modern));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
