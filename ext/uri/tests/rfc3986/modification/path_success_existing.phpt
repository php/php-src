--TEST--
Test Uri\Rfc3986\Uri component modification - path - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com/foo/bar");
$uri2 = $uri1->withPath("/baz");

var_dump($uri1->getRawPath());
var_dump($uri2->getRawPath());
var_dump($uri2->toRawString());
var_dump($uri2->getPath());
var_dump($uri2->toString());

?>
--EXPECT--
string(8) "/foo/bar"
string(4) "/baz"
string(23) "https://example.com/baz"
string(4) "/baz"
string(23) "https://example.com/baz"
