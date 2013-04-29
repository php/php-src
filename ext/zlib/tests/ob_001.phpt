--TEST--
zlib.output_compression
--SKIPIF--
<?php
if (!extension_loaded("zlib")) die("skip need ext/zlib");
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
‹%s

