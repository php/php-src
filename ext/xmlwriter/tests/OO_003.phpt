--TEST--
XMLWriter: libxml2 XML Writer, membuffer, flush, text, attribute
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php 
/* $Id$ */

$xw = new XMLWriter();
$xw->openMemory();
$xw->startDocument('1.0', 'UTF-8');
$xw->startElement("tag1");

$res = $xw->startAttribute('attr1');
$xw->text("attr1_value");
$xw->endAttribute();

$res = $xw->startAttribute('attr2');
$xw->text("attr2_value");
$xw->endAttribute();

$xw->text("Test text for tag1");
$res = $xw->startElement('tag2');
if ($res < 1) {
	echo "StartElement context validation failed\n";
	exit();
}
$xw->endDocument();

// Force to write and empty the buffer
echo $xw->flush(true);
?>
===DONE===
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<tag1 attr1="attr1_value" attr2="attr2_value">Test text for tag1<tag2/></tag1>
===DONE===
