--TEST--
Test Uri\Rfc3986\Uri component modification - scheme - basic case
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withScheme("http");

var_dump($uri1->getRawScheme());
var_dump($uri2->getRawScheme());
var_dump($uri2->getScheme());

?>
--EXPECT--
string(5) "https"
string(4) "http"
string(4) "http"
