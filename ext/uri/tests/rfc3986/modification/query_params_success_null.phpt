--TEST--
Test Uri\Rfc3986\Uri::withQueryParams() - success - null removes a non-empty query
--FILE--
<?php

$uri1 = new Uri\Rfc3986\Uri("https://example.com?foo=bar");

$uri2 = $uri1->withQueryParams(null);

var_dump($uri1->getRawQuery());
var_dump($uri2->getRawQuery());
var_dump($uri2->toRawString());
var_dump($uri2->getQuery());
var_dump($uri2->toString());

?>
--EXPECT--
string(7) "foo=bar"
NULL
string(19) "https://example.com"
NULL
string(19) "https://example.com"
