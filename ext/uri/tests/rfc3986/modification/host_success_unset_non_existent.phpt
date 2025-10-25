--TEST--
Test Uri\Rfc3986\Uri component modification - host - unsetting non-existent
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("/foo/bar");
$uri2 = $uri1->withHost(null);

var_dump($uri1->getRawHost());
var_dump($uri2->getRawHost());
var_dump($uri2->toRawString());
var_dump($uri2->getHost());
var_dump($uri2->toString());

?>
--EXPECT--
NULL
NULL
string(8) "/foo/bar"
NULL
string(8) "/foo/bar"
