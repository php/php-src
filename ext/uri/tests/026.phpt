--TEST--
Test property mutation - host
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withHost("test.com");
$uri3 = $uri2->withHost(null);

var_dump($uri1->toString());
var_dump($uri2->toString());
var_dump($uri3->toString());

$uri1 = Uri\Rfc3986\Uri::parse("ftp://foo.com?query=abc#foo");
$uri2 = $uri1->withHost("test.com");

var_dump($uri1->toString());
var_dump($uri2->toString());

$uri1 = Uri\Rfc3986\Uri::parse("/path?query=abc#foo");
$uri2 = $uri1->withHost("test.com");

var_dump($uri1->toString());
var_dump($uri2->toString());

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withHost("test.com");

var_dump($url1->toMachineFriendlyString());
var_dump($url2->toMachineFriendlyString());

try {
    $url2->withHost(null);
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

$url1 = Uri\WhatWg\Url::parse("ftp://foo.com?query=abc#foo");
$url2 = $url1->withHost("test.com");

var_dump($url1->toMachineFriendlyString());
var_dump($url2->toMachineFriendlyString());

?>
--EXPECT--
string(19) "https://example.com"
string(16) "https://test.com"
string(8) "https://"
string(27) "ftp://foo.com?query=abc#foo"
string(28) "ftp://test.com?query=abc#foo"
string(19) "/path?query=abc#foo"
string(27) "test.com/path?query=abc#foo"
string(20) "https://example.com/"
string(17) "https://test.com/"
URI parsing failed
string(28) "ftp://foo.com/?query=abc#foo"
string(29) "ftp://test.com/?query=abc#foo"
