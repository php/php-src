--TEST--
XMLWriter: libxml2 XML Writer, membuffer, flush, attribute
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php

$xw = xmlwriter_open_memory();
xmlwriter_start_document($xw, '1.0', 'UTF-8');
xmlwriter_start_element($xw, "tag1");


$res = xmlwriter_start_attribute($xw, 'attr1');
xmlwriter_text($xw, "attr1_value");
xmlwriter_end_attribute($xw);

xmlwriter_write_attribute($xw, "att2", "att2_value");
xmlwriter_text($xw, "Test text for tag1");
$res = xmlwriter_start_element($xw, 'tag2');
if ($res < 1) {
	echo "StartElement context validation failed\n";
	exit();
}
xmlwriter_end_document($xw);

// Force to write and empty the buffer
echo xmlwriter_flush($xw, true);
?>
===DONE===
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<tag1 attr1="attr1_value" att2="att2_value">Test text for tag1<tag2/></tag1>
===DONE===
