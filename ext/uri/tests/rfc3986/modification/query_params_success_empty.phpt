--TEST--
Test Uri\Rfc3986\Uri::withQueryParams() - success - empty list with an absent query
--FILE--
<?php

$uri1 = new Uri\Rfc3986\Uri("https://example.com");

$params = new Uri\QueryParams();

$uri2 = $uri1->withQueryParams($params);

var_dump($uri1->getRawQuery());
var_dump($uri2->getRawQuery());
var_dump($uri2->toRawString());
var_dump($uri2->getQuery());
var_dump($uri2->toString());

?>
--EXPECT--
NULL
string(0) ""
string(20) "https://example.com?"
string(0) ""
string(20) "https://example.com?"
