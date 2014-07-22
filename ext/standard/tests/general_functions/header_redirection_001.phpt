--TEST--
Location: headers change the status code
--CGI--
--FILE--
<?php
header('Location: http://example.com/');
?>
--EXPECTHEADERS--
Status: 302 Moved Temporarily
Location: http://example.com/
--EXPECT--
