--TEST--
xmlwriter_open_uri with non existing file
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php 
var_dump(xmlwriter_open_uri('foo/bar.tmp'));
?>
--CREDIT--
Koen Kuipers koenk82@gmail.com
Theo van der Zee
#Test Fest Utrecht 09-05-2009
--XFAIL--
Getting error: xmlNewTextWriterFilename : out of memory!
--EXPECTF--

Warning: xmlwriter_open_uri(%s): failed to open stream: No such file or directory in %s on line %d
bool(false)
