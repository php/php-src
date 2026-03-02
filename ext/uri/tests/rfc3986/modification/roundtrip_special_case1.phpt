--TEST--
Test Uri\Rfc3986\Uri component modification when roundtripping is not guaranteed - case 1
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("path1");
$uri2 = $uri1->withHost("host");
$uri2 = $uri2->withHost(null);

var_dump($uri1->getRawPath());
var_dump($uri2->getRawPath());
var_dump($uri2->toRawString());
var_dump($uri2->getPath());
var_dump($uri2->toString());

?>
--EXPECT--
string(5) "path1"
string(6) "/path1"
string(6) "/path1"
string(6) "/path1"
string(6) "/path1"
