--TEST--
zlib_get_coding_type() with deflate encoding
--CREDIT--
PHP TestFest 2017 - Bergfreunde, Florian Engelhardt <florian.engelhardt@bergfreunde.de>
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--ENV--
HTTP_ACCEPT_ENCODING=deflate
--FILE--
<?php
ini_set('zlib.output_compression', 'On');
$enc = zlib_get_coding_type();
ini_set('zlib.output_compression', 'Off');
var_dump($enc);
var_dump(zlib_get_coding_type());
?>
--EXPECT--
string(7) "deflate"
string(7) "deflate"
