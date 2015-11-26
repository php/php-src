--TEST--
Location: headers do not override the 303 See Other response code
--CGI--
--FILE--
<?php
header('HTTP/1.1 303 See Other');
header('Location: http://example.com/');
?>
--EXPECTHEADERS--
Status: 303 See Other
Location: http://example.com/
--EXPECT--
