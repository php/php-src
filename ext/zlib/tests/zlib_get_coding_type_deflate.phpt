--TEST--
zlib_get_coding_type() with deflate encoding
--CREDITS--
PHP TestFest 2017 - Bergfreunde, Florian Engelhardt <florian.engelhardt@bergfreunde.de>
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--ENV--
HTTP_ACCEPT_ENCODING=deflate
--FILE--
<?php
ini_set('zlib.output_compression', 'Off');
$encOff = zlib_get_coding_type();
ini_set('zlib.output_compression', 'On');
$encOn = zlib_get_coding_type();
ini_set('zlib.output_compression', 'Off');
var_dump($encOff);
var_dump($encOn);
?>
--EXPECT--
bool(false)
string(7) "deflate"
