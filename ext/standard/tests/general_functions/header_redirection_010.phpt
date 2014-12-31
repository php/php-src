--TEST--
Location: headers do not override the 304 Not Modified response code
--CGI--
--FILE--
<?php
header('HTTP/1.1 304 Not Modified');
header('Location: http://example.com/');
?>
--EXPECTHEADERS--
Status: 304 Not Modified
Location: http://example.com/
--EXPECT--
