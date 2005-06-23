--TEST--
XMLWriter: libxml2 XML Writer, comments 
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php 
/* $Id$ */

$doc_dest = '001.xml';
$xw = new XMLWriter();
$xw->openUri($doc_dest);
$xw->startDocument('1.0', 'utf8');
$xw->startElement("tag1");
$xw->startComment();
$xw->text('comment');
$xw->endComment();
$xw->writeComment("comment #2");
$xw->endDocument();

// Force to write and empty the buffer
$output_bytes = $xw->flush(true);
echo file_get_contents($doc_dest);
unlink('001.xml');
echo "---Done---\n";
?>
--EXPECT--
<?xml version="1.0" encoding="utf8"?>
<tag1><!--comment--><!--comment #2--></tag1>
---Done--- 
