--TEST--
Test Uri\Rfc3986\Uri component modification - host - adding a new one
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("/foo/bar");
$uri2 = $uri1->withHost("example.com");

var_dump($uri1->getRawHost());
var_dump($uri2->getRawHost());
var_dump($uri2->toRawString());
var_dump($uri2->getHost());
var_dump($uri2->toString());

?>
--EXPECT--
NULL
string(11) "example.com"
string(21) "//example.com/foo/bar"
string(11) "example.com"
string(21) "//example.com/foo/bar"
