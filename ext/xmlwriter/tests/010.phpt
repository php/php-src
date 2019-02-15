--TEST--
xmlwriter_start/end_attribute()
--SKIPIF--
<?php
if (!extension_loaded("xmlwriter")) die("skip");
?>
--FILE--
<?php

$file = dirname(__FILE__).'/010.tmp';

$xw = xmlwriter_open_uri($file);

var_dump(xmlwriter_start_element($xw, "tag"));
var_dump(xmlwriter_start_attribute($xw, "attr"));
var_dump(xmlwriter_end_attribute($xw));
var_dump(xmlwriter_start_attribute($xw, "-1"));
var_dump(xmlwriter_end_attribute($xw));
var_dump(xmlwriter_start_attribute($xw, "\""));
var_dump(xmlwriter_end_attribute($xw));
var_dump(xmlwriter_end_element($xw));

unset($xw);

var_dump(file_get_contents($file));

@unlink($file);

echo "Done\n";
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)

Warning: xmlwriter_start_attribute(): Invalid Attribute Name in %s on line %d
bool(false)
bool(false)

Warning: xmlwriter_start_attribute(): Invalid Attribute Name in %s on line %d
bool(false)
bool(false)
bool(true)
string(14) "<tag attr=""/>"
Done
