--TEST--
Location: headers do not override the 305 Use Proxy response code
--CGI--
--FILE--
<?php
header('HTTP/1.1 305 Use Proxy');
header('Location: http://example.com/');
?>
--EXPECTHEADERS--
Status: 305 Use Proxy
Location: http://example.com/
--EXPECT--
