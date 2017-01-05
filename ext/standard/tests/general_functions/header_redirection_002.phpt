--TEST--
Location: headers override non-201 and 3xx response codes
--CGI--
--FILE--
<?php
header("HTTP/1.1 418 I'm a Teapot");
header('Location: http://example.com/');
?>
--EXPECTHEADERS--
Status: 302 Found
Location: http://example.com/
--EXPECT--
