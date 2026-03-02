--TEST--
Bug #81518 (Header injection via default_mimetype / default_charset)
--CGI--
--FILE--
<?php
ini_set(
    "default_mimetype",
    "text/html;charset=ISO-8859-1\r\nContent-Length: 31\r\n\r\n" .
    "Lets smuggle a HTTP response.\r\n"
);
?>
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
