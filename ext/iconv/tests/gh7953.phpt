--TEST--
GH-7953 (ob_clean() only may not set Content-* header)
--SKIPIF--
<?php
if (!extension_loaded("iconv")) die("skip iconv extension not available");
?>
--INI--
input_encoding=UTF-8
output_encoding=ISO-8859-1
--CGI--
--FILE--
<?php
ob_start('ob_iconv_handler');
ob_clean();

echo 'Hello World';
?>
--EXPECTF--
%a
--EXPECTHEADERS--
Content-Type: text/html; charset=ISO-8859-1
