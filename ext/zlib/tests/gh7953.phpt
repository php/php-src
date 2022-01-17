--TEST--
GH-7953 (ob_clean() only does not set Content-Encoding)
--SKIPIF--
<?php
if (!extension_loaded("zlib")) die("skip zlib extension not available");
?>
--CGI--
--ENV--
HTTP_ACCEPT_ENCODING=gzip
--FILE--
<?php
ob_start('ob_gzhandler');
ob_clean();

echo 'Hello World';
?>
--EXPECTF--
%a
--EXPECTHEADERS--
Content-Encoding: gzip
Vary: Accept-Encoding
