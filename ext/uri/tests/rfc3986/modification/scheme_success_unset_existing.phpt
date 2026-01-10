--TEST--
Test Uri\Rfc3986\Uri component modification - scheme - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withScheme(null);

var_dump($uri1->getRawScheme());
var_dump($uri2->getRawScheme());
var_dump($uri2->toRawString());
var_dump($uri2->getScheme());
var_dump($uri2->toString());

?>
--EXPECT--
string(5) "https"
NULL
string(13) "//example.com"
NULL
string(13) "//example.com"
