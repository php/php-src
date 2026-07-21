--TEST--
Test property mutation - scheme
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
var_dump($uri1->getRawScheme());
var_dump($uri1->getScheme());

$uri2 = $uri1->withScheme("http");
var_dump($uri2->getRawScheme());
var_dump($uri2->getScheme());

$uri3 = $uri2->withScheme(null);
var_dump($uri3->getRawScheme());
var_dump($uri3->getScheme());

try {
    $uri3->withScheme("");
} catch (Uri\InvalidUriException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $uri3->withScheme("http%73");
} catch (Uri\InvalidUriException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withScheme("http");

var_dump($url1->getScheme());
var_dump($url2->getScheme());

try {
    $url2->withScheme("");
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $url2->withScheme("http%73");
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
string(5) "https"
string(5) "https"
string(4) "http"
string(4) "http"
NULL
NULL
Uri\InvalidUriException: The specified scheme is malformed
Uri\InvalidUriException: The specified scheme is malformed
string(5) "https"
string(4) "http"
Uri\WhatWg\InvalidUrlException: The specified scheme is malformed
Uri\WhatWg\InvalidUrlException: The specified scheme is malformed
