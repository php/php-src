--TEST--
Location: headers do not override the 399 Choose Your Own Adventure response code
--CGI--
--FILE--
<?php
header('HTTP/1.1 399 Choose Your Own Adventure');
header('Location: http://example.com/');
?>
--EXPECTHEADERS--
Status: 399 Choose Your Own Adventure
Location: http://example.com/
--EXPECT--
