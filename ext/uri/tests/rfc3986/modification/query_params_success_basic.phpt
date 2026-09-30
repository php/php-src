--TEST--
Test Uri\Rfc3986\Uri::withQueryParams() - success - replace a non-empty query
--FILE--
<?php

$uri1 = new Uri\Rfc3986\Uri("https://example.com?old=1");

$params = Uri\QueryParams::parseRfc3986("foo=bar&foo=baz");

$uri2 = $uri1->withQueryParams($params);

var_dump($uri1->getRawQuery());
var_dump($uri2->getRawQuery());
var_dump($uri2->toRawString());
var_dump($uri2->getQuery());
var_dump($uri2->toString());

?>
--EXPECT--
string(5) "old=1"
string(15) "foo=bar&foo=baz"
string(35) "https://example.com?foo=bar&foo=baz"
string(15) "foo=bar&foo=baz"
string(35) "https://example.com?foo=bar&foo=baz"
