--TEST--
XMLWriter: libxml2 XML Writer, membuffer, flush
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php

$xw = new XMLWriter();
$xw->openMemory();
$xw->startDocument('1.0', 'UTF-8', 'standalone');
$xw->startElement("tag1");
$xw->endDocument();

// Force to write and empty the buffer
echo $xw->flush(true);
?>
===DONE===
--EXPECT--
<?xml version="1.0" encoding="UTF-8" standalone="standalone"?>
<tag1/>
===DONE===
