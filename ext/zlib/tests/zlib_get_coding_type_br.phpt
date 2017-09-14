--TEST--
zlib_get_coding_type() with unsupported encoding
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--ENV--
HTTP_ACCEPT_ENCODING=br
--FILE--
<?php
ini_set('zlib.output_compression', 'On');
$enc = zlib_get_coding_type();
ini_set('zlib.output_compression', 'Off');
var_dump($enc);
var_dump(zlib_get_coding_type());
?>
--EXPECT--
bool(false)
bool(false)
