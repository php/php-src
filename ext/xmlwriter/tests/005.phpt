--TEST--
XMLWriter: libxml2 XML Writer, comments 
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php 
/* $Id$ */

$doc_dest = '001.xml';
$xw = xmlwriter_open_uri($doc_dest);
xmlwriter_start_document($xw, '1.0', 'utf8');
xmlwriter_start_element($xw, "tag1");

xmlwriter_start_comment($xw);
xmlwriter_text($xw, 'comment');
xmlwriter_end_comment($xw);
xmlwriter_write_comment($xw, "comment #2");
xmlwriter_end_document($xw);

// Force to write and empty the buffer
$output_bytes = xmlwriter_flush($xw, true);
echo file_get_contents($doc_dest);
unlink('001.xml');
echo "---Done---\n";
?>
--EXPECT--
<?xml version="1.0" encoding="utf8"?>
<tag1><!--comment--><!--comment #2--></tag1>
---Done--- 
