--TEST--
XMLReader: libxml2 XML Reader, string data
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip";
$reader = new XMLReader();
if (!method_exists($reader, 'readInnerXml')) print "skip";
?>
--FILE--
<?php

$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books><book>test</book></books>';

$reader = new XMLReader();
$reader->XML($xmlstring);
$reader->read();
echo $reader->readInnerXml();
echo "\n";
$reader->close();


$reader = new XMLReader();
$reader->XML($xmlstring);
$reader->read();
echo $reader->readOuterXml();
echo "\n";
$reader->close();
?>
--EXPECT--
<book>test</book>
<books><book>test</book></books>
