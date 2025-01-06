--TEST--
Test property mutation - fragment
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com#fragment1");
$uri2 = $uri1->withFragment("#fragment2");
$uri3 = $uri2->withFragment(null);

var_dump($uri1->toString());
var_dump($uri2->toString());
var_dump($uri3->toString());

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com?abc=def");
$uri2 = $uri1->withFragment("#fragment");

var_dump($uri1->toString());
var_dump($uri2->toString());

$url1 = Uri\WhatWg\Url::parse("https://example.com#fragment1");
$url2 = $url1->withFragment("#fragment2");
$url3 = $url2->withFragment(null);

var_dump($url1->toMachineFriendlyString());
var_dump($url2->toMachineFriendlyString());
var_dump($url3->toMachineFriendlyString());

$url1 = Uri\WhatWg\Url::parse("https://example.com?abc=def");
$url2 = $url1->withFragment("#fragment");

var_dump($url1->toMachineFriendlyString());
var_dump($url2->toMachineFriendlyString());

?>
--EXPECT--
string(29) "https://example.com#fragment1"
string(29) "https://example.com#fragment2"
string(19) "https://example.com"
string(27) "https://example.com?abc=def"
string(36) "https://example.com?abc=def#fragment"
string(30) "https://example.com/#fragment1"
string(30) "https://example.com/#fragment2"
string(20) "https://example.com/"
string(28) "https://example.com/?abc=def"
string(37) "https://example.com/?abc=def#fragment"
