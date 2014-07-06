--TEST--
Location: headers do not override the 201 response code
--CGI--
--FILE--
<?php
header('HTTP/1.1 201 Created');
header('Location: http://example.com/');
?>
--EXPECTHEADERS--
Status: 201 Created
Location: http://example.com/
--EXPECT--
