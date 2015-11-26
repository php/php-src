--TEST--
Location: headers do not override the 300 Multiple Choices response code
--CGI--
--FILE--
<?php
header('HTTP/1.1 300 Multiple Choices');
header('Location: http://example.com/');
?>
--EXPECTHEADERS--
Status: 300 Multiple Choices
Location: http://example.com/
--EXPECT--
