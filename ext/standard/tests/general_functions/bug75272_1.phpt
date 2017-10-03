--TEST--
WWW-Authenticate: header does not override response code when response code set before header
--XFAIL--
Bug #75272 might still be open. This test asserts the expected behavior of WWW-Authenticate after the bug is resolved
--CGI--
--FILE--
<?php
header('HTTP/1.1 399 Choose Your Own Adventure');
header('WWW-Authenticate: Basic realm="Foo"');
?>
--EXPECTHEADERS--
Status: 399 Choose Your Own Adventure
WWW-Authenticate: Basic realm="Foo"
--EXPECT--
