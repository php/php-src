--TEST--
Test Uri\Rfc3986\Uri component modification - query - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com?foo=bar");
$uri2 = $uri1->withQuery(null);

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
