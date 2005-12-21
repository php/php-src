--TEST--
XMLReader: libxml2 XML Reader, next 
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip"; ?>
--FILE--
<?php 
/* $Id$ */

$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books><book num="1"><test /></book><book num="2" /></books>';

$reader = new XMLReader();
$reader->XML($xmlstring);

// Only go through
$reader->read();
$reader->read();

$reader->next();
echo $reader->name;
echo " ";
echo $reader->getAttribute('num');
echo "\n";
?>
===DONE===
--EXPECTF--
book 2
===DONE===
