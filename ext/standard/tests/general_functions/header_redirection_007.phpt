--TEST--
Location: headers do not override the 301 Moved Permanently response code
--CGI--
--FILE--
<?php
header('HTTP/1.1 301 Moved Permanently');
header('Location: http://example.com/');
?>
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: http://example.com/
--EXPECT--
