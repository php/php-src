--TEST--
Location: headers do not override the 307 Temporary Redirect response code
--CGI--
--FILE--
<?php
header('HTTP/1.1 307 Temporary Redirect');
header('Location: http://example.com/');
?>
--EXPECTHEADERS--
Status: 307 Temporary Redirect
Location: http://example.com/
--EXPECT--
