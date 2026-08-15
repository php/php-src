--TEST--
Test Uri\Rfc3986\Uri reference resolution - resolve() - success
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://example.com");

var_dump($uri->resolve("/foo/")->toString());
var_dump($uri->resolve("https://test.com/foo")->toString());

?>
--EXPECTF--
string(24) "https://example.com/foo/"
string(20) "https://test.com/foo"
