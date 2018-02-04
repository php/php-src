--TEST--
zlib_get_coding_type() basic call without env
--CREDITS--
PHP TestFest 2017 - Bergfreunde, Florian Engelhardt <florian.engelhardt@bergfreunde.de>
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
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
bool(false)
