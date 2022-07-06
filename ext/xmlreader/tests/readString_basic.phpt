--TEST--
XMLReader: readString basic
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip XMLReader extension required"; ?>
--FILE--
<?php

$xml = '<?xml version="1.0" encoding="UTF-8"?><books><book>Book1</book><book>Book2</book></books>';

$reader = new XMLReader();
$reader->xml($xml);
$reader->read();
echo $reader->readString();
?>
--EXPECT--
Book1Book2
