--TEST--
Location: headers do not override the 308 Permanent Redirect response code
--CGI--
--FILE--
<?php
header('HTTP/1.1 308 Permanent Redirect');
header('Location: http://example.com/');
?>
--EXPECTHEADERS--
Status: 308 Permanent Redirect
Location: http://example.com/
--EXPECT--
