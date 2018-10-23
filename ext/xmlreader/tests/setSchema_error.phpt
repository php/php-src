--TEST--
XMLReader: setSchema Error
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip XMLReader extension required"; ?>
--FILE--
<?php

$reader = new XMLReader();
var_dump($reader->setSchema(''));
$reader->close();

$reader = new XMLReader();
var_dump($reader->setSchema('schema-missing-file.xsd'));
$reader->close();

$reader = new XMLReader();
var_dump($reader->setSchema('schema-empty.xsd'));
$reader->close();
?>
--EXPECTF--

Warning: XMLReader::setSchema(): Schema data source is required in %s on line %d
bool(false)

Warning: XMLReader::setSchema(): Unable to set schema. This must be set prior to reading or schema contains errors. in %s on line %d
bool(false)

Warning: XMLReader::setSchema(): Unable to set schema. This must be set prior to reading or schema contains errors. in %s on line %d
bool(false)
