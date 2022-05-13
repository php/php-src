--TEST--
zlib.output_compression
--EXTENSIONS--
zlib
--SKIPIF--
<?php
if (false === stristr(PHP_SAPI, "cgi")) die("skip need sapi/cgi");
?>
--GET--
a=b
--INI--
zlib.output_compression=1
--ENV--
HTTP_ACCEPT_ENCODING=gzip
--FILE--
<?php
echo "hi\n";
?>
--EXPECTF--
‹%a
