--TEST--
WWW-Authenticate: header automatically sets 401 Unauthorized if no status codes previously set
--CGI--
--FILE--
<?php
header('WWW-Authenticate: Basic realm="Foo"');
?>
--EXPECTHEADERS--
Status: 401 Unauthorized
WWW-Authenticate: Basic realm="Foo"
--EXPECT--
