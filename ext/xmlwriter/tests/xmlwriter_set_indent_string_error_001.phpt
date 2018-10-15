--TEST--
xmlwriter_set_indent_string with missing param(s)
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php
$temp_filename = dirname(__FILE__)."/xmlwriter_set_indent_string_error.tmp";
	$fp = fopen($temp_filename, "w");
	fwrite ($fp, "Hi");
	fclose($fp);
$resource = xmlwriter_open_uri($temp_filename);
var_dump(xmlwriter_set_indent_string($resource));
?>
--CLEAN--
<?php
$temp_filename = dirname(__FILE__)."/xmlwriter_set_indent_string_error.tmp";
unlink($temp_filename);
?>
--CREDITS--
Koen Kuipers koenk82@gmail.com
--EXPECTF--
Warning: xmlwriter_set_indent_string() expects exactly %d parameters, %d given in %s on line %d
NULL
