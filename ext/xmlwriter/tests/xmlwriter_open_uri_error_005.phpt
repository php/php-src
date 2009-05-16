--TEST--
xmlwriter_open_uri with file://localhost/
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php 
var_dump(xmlwriter_open_uri('file://localhost/'));
?>
--CREDIT--
Koen Kuipers koenk82@gmail.com
Theo van der Zee
#Test Fest Utrecht 09-05-2009
--XFAIL--
Getting error: xmlNewTextWriterFilename : out of memory!
--EXPECTF--

Warning: xmlwriter_open_uri(/): failed to open stream: Is a directory in %s on line %d
bool(false)
