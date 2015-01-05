--TEST--
XMLWriter: libxml2 XML Writer, membuffer, flush
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php 
/* $Id$ */

$doc_dest = '001.xml';
$xw = xmlwriter_open_memory($doc_dest);
xmlwriter_start_document($xw, '1.0', 'UTF-8');
xmlwriter_start_element($xw, "tag1");
xmlwriter_end_document($xw);

// Force to write and empty the buffer
echo xmlwriter_flush($xw, true);
?>
===DONE===
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<tag1/>
===DONE===
