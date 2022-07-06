--TEST--
Location: headers do not override the 302 Found response code
--CGI--
--FILE--
<?php
header('HTTP/1.1 302 Found');
header('Location: http://example.com/');
?>
--EXPECTHEADERS--
Status: 302 Found
Location: http://example.com/
--EXPECT--
