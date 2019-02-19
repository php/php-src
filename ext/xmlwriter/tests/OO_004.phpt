--TEST--
XMLWriter: libxml2 XML Writer, file buffer, flush
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php

$doc_dest = 'OO_004.xml';
$xw = new XMLWriter();
$xw->openUri($doc_dest);
$xw->startDocument('1.0', 'UTF-8');
$xw->startElement("tag1");

$xw->startPi("PHP");
$xw->text('echo $a;');
$xw->endPi();
$xw->endDocument();

// Force to write and empty the buffer
$xw->flush(true);
$md5_out = md5_file($doc_dest);
$md5_res = md5('<?xml version="1.0" encoding="UTF-8"?>
<tag1><?PHP echo $a;?></tag1>
');
unset($xw);
unlink($doc_dest);
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
