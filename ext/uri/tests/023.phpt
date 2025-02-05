--TEST--
Test property mutation - scheme
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withScheme("http");
$uri3 = $uri2->withScheme(null);

var_dump($uri1->toString());
var_dump($uri2->toString());
var_dump($uri3->toString());

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withScheme("http");

var_dump($url1->toString());
var_dump($url2->toString());

try {
    $url2->withScheme("");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
string(19) "https://example.com"
string(18) "http://example.com"
string(11) "example.com"
string(20) "https://example.com/"
string(19) "http://example.com/"
URI parsing failed
