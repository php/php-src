--TEST--
Test Uri\Rfc3986\Uri component modification - query - context-sensitive reserved character
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withQuery("?foo=bar");

var_dump($uri1->getRawQuery());
var_dump($uri2->getRawQuery());
var_dump($uri2->toRawString());
var_dump($uri2->getQuery());
var_dump($uri2->toString());

?>
--EXPECT--
NULL
string(8) "?foo=bar"
string(28) "https://example.com??foo=bar"
string(8) "?foo=bar"
string(28) "https://example.com??foo=bar"
