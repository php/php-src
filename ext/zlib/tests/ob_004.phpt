--TEST--
ob_gzhandler
--EXTENSIONS--
zlib
--SKIPIF--
<?php
if (false === stristr(PHP_SAPI, "cgi")) die("skip need sapi/cgi");
?>
--INI--
zlib.output_compression=0
--ENV--
HTTP_ACCEPT_ENCODING=gzip
--POST--
dummy=42
--FILE--
<?php
ob_start("ob_gzhandler");
echo "hi\n";
?>
--EXPECTF--
‹%a
--EXPECTHEADERS--
Content-Encoding: gzip
Vary: Accept-Encoding
