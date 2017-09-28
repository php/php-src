--TEST--
WWW-Authenticate: header does not override response code when response code set after header
--CGI--
--FILE--
<?php
header('WWW-Authenticate: Basic realm="Foo"');
header('HTTP/1.1 399 Choose Your Own Adventure');
?>
--EXPECTHEADERS--
Status: 399 Choose Your Own Adventure
WWW-Authenticate: Basic realm="Foo"
--EXPECT--

