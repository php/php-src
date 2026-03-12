--TEST--
Test Uri\Rfc3986\Uri component modification - fragment - empty string
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withFragment("");

var_dump($uri1->getRawFragment());
var_dump($uri2->getRawFragment());
var_dump($uri2->toRawString());
var_dump($uri2->getFragment());
var_dump($uri2->toString());

?>
--EXPECT--
NULL
string(0) ""
string(20) "https://example.com#"
string(0) ""
string(20) "https://example.com#"
