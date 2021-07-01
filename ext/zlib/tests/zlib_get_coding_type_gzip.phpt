--TEST--
zlib_get_coding_type() with gzip encoding
--CREDITS--
PHP TestFest 2017 - Bergfreunde, Florian Engelhardt <florian.engelhardt@bergfreunde.de>
--EXTENSIONS--
zlib
--ENV--
HTTP_ACCEPT_ENCODING=gzip
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
string(4) "gzip"
