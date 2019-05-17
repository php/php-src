--TEST--
XMLWriter: libxml2 XML Writer, file buffer, flush
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php

$doc_dest = 'OO_001.xml';
$xw = new XMLWriter();
$xw->openUri($doc_dest);
$xw->startDocument('1.0', 'UTF-8', 'standalonearg');
$xw->startElement("tag1");
$xw->endDocument();

// Force to write and empty the buffer
$output_bytes = $xw->flush(true);
echo file_get_contents($doc_dest);
unset($xw);
unlink($doc_dest);
?>
===DONE===
--EXPECT--
<?xml version="1.0" encoding="UTF-8" standalone="standalonearg"?>
<tag1/>
===DONE===
