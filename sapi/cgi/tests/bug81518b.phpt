--TEST--
Bug #81518 (Header injection via default_mimetype / default_charset)
--CGI--
--FILE--
<?php
ini_set('default_charset', 'ISO-8859-1' . "\r\nHeader-Injection: Works!");
header('Content-Type: text/html');
?>
--EXPECTHEADERS--
Content-type: text/html;charset=UTF-8
--EXPECT--
