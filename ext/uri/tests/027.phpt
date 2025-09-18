--TEST--
Test property mutation - port
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com:8080");
$uri2 = $uri1->withPort(22);
$uri3 = $uri2->withPort(null);

var_dump($uri1->getPort());
var_dump($uri2->getPort());
var_dump($uri3->getPort());

$uri1 = Uri\Rfc3986\Uri::parse("ftp://foo.com:443?query=abc#foo");
$uri2 = $uri1->withPort(8080);

var_dump($uri1->getPort());
var_dump($uri2->getPort());

$uri1 = Uri\Rfc3986\Uri::parse("file:///foo/bar");
$uri2 = $uri1->withPort(80);

var_dump($uri1->getPort());
var_dump($uri2->getPort());

$uri1 = Uri\Rfc3986\Uri::parse("/foo");
$uri2 = $uri1->withPort(null);
var_dump($uri2->getPort());

try {
    $uri1->withPort(1);
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

$url1 = Uri\WhatWg\Url::parse("https://example.com:8080");
$url2 = $url1->withPort(22);
$url3 = $url2->withPort(null);

var_dump($url1->getPort());
var_dump($url2->getPort());
var_dump($url3->getPort());

$url1 = Uri\WhatWg\Url::parse("ftp://foo.com:443?query=abc#foo");
$url2 = $url1->withPort(8080);

var_dump($url1->getPort());
var_dump($url2->getPort());

$url1 = Uri\WhatWg\Url::parse("file:///foo/bar");
$url2 = $url1->withPort(80);

var_dump($url1->getPort());
var_dump($url2->getPort());

?>
--EXPECT--
int(8080)
int(22)
NULL
int(443)
int(8080)
NULL
int(80)
NULL
Cannot set a port without having a host
int(8080)
int(22)
NULL
int(443)
int(8080)
NULL
NULL
