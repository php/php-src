--TEST--
Bug #42090 (json_decode causes segmentation fault)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
var_dump(
	json_decode('""'),
	json_decode('"..".'),
	json_decode('"'),
	json_decode('""""'),
	json_encode('"'),
	json_decode(json_encode('"')),
	json_decode(json_encode('""'))
);
?>
--EXPECT--
unicode(0) ""
unicode(5) "".."."
unicode(1) """
unicode(2) """"
string(4) ""\"""
unicode(1) """
unicode(2) """"

