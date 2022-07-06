--TEST--
Location: headers respect the header() response code parameter
--CGI--
--FILE--
<?php
header('Location: http://example.com/', true, 404);
?>
--EXPECTHEADERS--
Status: 404 Not Found
Location: http://example.com/
--EXPECT--
