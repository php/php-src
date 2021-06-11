--TEST--
zlib_get_coding_type() with unsupported encoding
--CREDITS--
PHP TestFest 2017 - Bergfreunde, Florian Engelhardt <florian.engelhardt@bergfreunde.de>
--EXTENSIONS--
zlib
--ENV--
HTTP_ACCEPT_ENCODING=br
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
