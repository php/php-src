--TEST--
XMLWriter: libxml2 XML Writer, comments
--SKIPIF--
<?php
if (!extension_loaded("xmlwriter")) die("skip");
if (!function_exists("xmlwriter_start_comment")) die("skip: libxml2 2.6.7+ required");
?>
--FILE--
<?php

$doc_dest = 'OO_005.xml';
$xw = new XMLWriter();
$xw->openUri($doc_dest);
$xw->startDocument('1.0', 'UTF-8');
$xw->startElement("tag1");
$xw->startComment();
$xw->text('comment');
$xw->endComment();
$xw->writeComment("comment #2");
$xw->endDocument();

// Force to write and empty the buffer
$output_bytes = $xw->flush(true);
echo file_get_contents($doc_dest);
unset($xw);
unlink($doc_dest);
?>
===DONE===
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<tag1><!--comment--><!--comment #2--></tag1>
===DONE===
