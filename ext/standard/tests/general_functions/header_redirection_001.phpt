--TEST--
Location: headers change the status code
--CGI--
--FILE--
<?php
header('Location: http://example.com/');
?>
--EXPECTHEADERS--
Status: 302 Found
Location: http://example.com/
--EXPECT--
