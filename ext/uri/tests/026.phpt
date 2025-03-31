--TEST--
Test property mutation - host
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withHost("test.com");
$uri3 = $uri2->withHost(null);
$uri4 = $uri3->withHost("t%65st.com"); // test.com
$uri5 = $uri4->withHost("t%3As%2Ft.com"); // t:s/t.com
$uri6 = $uri5->withHost("test.com:8080"); // TODO should fail

var_dump($uri2->getRawHost());
var_dump($uri3->getRawHost());
var_dump($uri4->getRawHost());
var_dump($uri5->getRawHost());
var_dump($uri6->getRawHost());

try {
    $uri4->withHost("t:s/t.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

$uri1 = Uri\Rfc3986\Uri::parse("ftp://foo.com?query=abc#foo");
$uri2 = $uri1->withHost("test.com");

var_dump($uri1->getRawHost());
var_dump($uri2->getRawHost());

$uri1 = Uri\Rfc3986\Uri::parse("/path?query=abc#foo");
$uri2 = $uri1->withHost("test.com");

var_dump($uri1->getRawHost());
var_dump($uri2->getRawHost());

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withHost("test.com");
$url3 = $url2->withHost("t%65st.com"); // test.com
$url4 = $url3->withHost("test.com:8080");

var_dump($url1->getAsciiHost());
var_dump($url2->getAsciiHost());
var_dump($url3->getAsciiHost());
var_dump($url4->getAsciiHost());
var_dump($url4->getPort());

try {
    $url4->withHost("t%3As%2Ft.com"); // t:s/t.com
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $url4->withHost("t:s/t.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $url2->withHost(null);
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

$url1 = Uri\WhatWg\Url::parse("ftp://foo.com?query=abc#foo");
$url2 = $url1->withHost("test.com");

var_dump($url1->getAsciiHost());
var_dump($url2->getAsciiHost());

?>
--EXPECT--
string(8) "test.com"
NULL
string(10) "t%65st.com"
string(13) "t%3As%2Ft.com"
string(8) "test.com"
URI parsing failed
string(7) "foo.com"
string(8) "test.com"
NULL
NULL
string(11) "example.com"
string(8) "test.com"
string(8) "test.com"
string(8) "test.com"
NULL
URI parsing failed
URI parsing failed
string(7) "foo.com"
string(8) "test.com"
