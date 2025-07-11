--TEST--
zlib.output_compression
--EXTENSIONS--
zlib
--SKIPIF--
--CGI--
--INI--
zlib.output_compression=0
--ENV--
HTTP_ACCEPT_ENCODING=gzip
--POST--
dummy=42
--FILE--
<?php
ini_set("zlib.output_compression", 1);
echo "hi\n";
?>
--EXPECTF--
‹%a
--EXPECTHEADERS--
Content-Encoding: gzip
Vary: Accept-Encoding
