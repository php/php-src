--TEST--
Test Uri\Rfc3986\Uri component modification when roundtripping is not guaranteed - case 3
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("//host//path");
$uri2 = $uri1->withHost(null);
$uri2 = $uri2->withHost("host");

var_dump($uri1->toRawString());
var_dump($uri2->toRawString());
var_dump($uri2->toString());

?>
--EXPECT--
string(12) "//host//path"
string(14) "//host/.//path"
string(12) "//host//path"
