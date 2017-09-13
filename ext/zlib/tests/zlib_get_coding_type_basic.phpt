--TEST--
test zlib_get_coding_type()
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--INI--
zlib.output_compression = Off
--FILE--
<?php var_dump(zlib_get_coding_type()); ?>
--EXPECT--
bool(false)
