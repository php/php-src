--TEST--
Test Uri\Rfc3986\Uri component modification - path - empty string
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withPath("");

var_dump($uri1->getRawPath());
var_dump($uri2->getRawPath());
var_dump($uri2->toRawString());
var_dump($uri2->getPath());
var_dump($uri2->toString());

?>
--EXPECT--
string(0) ""
string(0) ""
string(19) "https://example.com"
string(0) ""
string(19) "https://example.com"
