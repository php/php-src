--TEST--
XMLWriter: libxml2 XML Writer, file buffer, flush
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php
/* $Id$ */

$doc_dest = '001.xml';
$xw = xmlwriter_open_uri($doc_dest);
xmlwriter_start_document($xw, '1.0', 'UTF-8');
xmlwriter_start_element($xw, "tag1");

xmlwriter_start_pi($xw, "PHP");
xmlwriter_text($xw, 'echo $a;');
xmlwriter_end_pi($xw);
xmlwriter_end_document($xw);

// Force to write and empty the buffer
$output_bytes = xmlwriter_flush($xw, true);
$md5_out = md5_file($doc_dest);
$md5_res = md5('<?xml version="1.0" encoding="UTF-8"?>
<tag1><?PHP echo $a;?></tag1>
');
unset($xw);
unlink('001.xml');
if ($md5_out != $md5_res) {
	echo "failed: $md5_res != $md5_out\n";
} else {
	echo "ok.\n";
}
?>
===DONE===
--EXPECT--
ok.
===DONE===
