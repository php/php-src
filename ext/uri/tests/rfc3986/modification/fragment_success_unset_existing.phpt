--TEST--
Test Uri\Rfc3986\Uri component modification - fragment - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com#foo");
$uri2 = $uri1->withFragment(null);

var_dump($uri1->getRawFragment());
var_dump($uri2->getRawFragment());
var_dump($uri2->toRawString());
var_dump($uri2->getFragment());
var_dump($uri2->toString());

?>
--EXPECT--
string(3) "foo"
NULL
string(19) "https://example.com"
NULL
string(19) "https://example.com"
