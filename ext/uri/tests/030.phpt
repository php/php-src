--TEST--
Test property mutation - fragment
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com#fragment1");
var_dump($uri1->getRawFragment());
var_dump($uri1->getFragment());

$uri2 = $uri1->withFragment("fragment2");
var_dump($uri2->getRawFragment());
var_dump($uri2->getFragment());

$uri3 = $uri2->withFragment(null);
var_dump($uri3->getRawFragment());
var_dump($uri3->getFragment());

try {
    $uri3->withFragment(" ");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $uri1->withFragment("#fragment2");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com?abc=def");
$uri2 = $uri1->withFragment("fragment");

var_dump($uri1->getFragment());
var_dump($uri2->getFragment());

$url1 = Uri\WhatWg\Url::parse("https://example.com#fragment1");
$url2 = $url1->withFragment("#fragment2");
$url3 = $url2->withFragment(null);
$url4 = $url3->withFragment(" ");

var_dump($url1->getFragment());
var_dump($url2->getFragment());
var_dump($url3->getFragment());
var_dump($url4->getFragment());

$url1 = Uri\WhatWg\Url::parse("https://example.com?abc=def");
$url2 = $url1->withFragment("#fragment");

var_dump($url1->getFragment());
var_dump($url2->getFragment());

?>
--EXPECT--
string(9) "fragment1"
string(9) "fragment1"
string(9) "fragment2"
string(9) "fragment2"
NULL
NULL
The specified fragment is malformed
The specified fragment is malformed
NULL
string(8) "fragment"
string(9) "fragment1"
string(9) "fragment2"
NULL
string(3) "%20"
NULL
string(8) "fragment"
