--TEST--
GH-7953 (ob_clean() only may not set Content-* header)
--EXTENSIONS--
iconv
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
