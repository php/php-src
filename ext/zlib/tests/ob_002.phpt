--TEST--
zlib.output_compression
--SKIPIF--
<?php
if (!extension_loaded("zlib")) die("skip need ext/zlib");
?>
--INI--
zlib.output_compression=1
--ENV--
HTTP_ACCEPT_ENCODING=gzip
--FILE--
<?php
ini_set("zlib.output_compression", 0);
echo "hi\n";
?>
--EXPECT--
hi
