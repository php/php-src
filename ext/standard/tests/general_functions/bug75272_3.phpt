--TEST--
WWW-Authenticate: header does not override the 399 response code when code passed as third argument
--CGI--
--FILE--
<?php
header('WWW-Authenticate: Basic realm="Foo"', true, 399);
?>
--EXPECTHEADERS--
Status: 399
WWW-Authenticate: Basic realm="Foo"
--EXPECT--
