--TEST--
Test toNormalizedString()
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("HTTPS://////EXAMPLE.com");
$uri2 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri3 = Uri\Rfc3986\Uri::parse("https://example.com/foo/..");
var_dump($uri1->toString());
var_dump($uri1->toNormalizedString());
var_dump($uri2->toString());
var_dump($uri2->toNormalizedString());
var_dump($uri3->toString());
var_dump($uri3->toNormalizedString());

$url1 = Uri\WhatWg\Url::parse("HTTPS://////EXAMPLE.com");
$url2 = Uri\WhatWg\Url::parse("https://example.com");
$url3 = Uri\WhatWg\Url::parse("https://example.com/foo/..");

var_dump($url1->toDisplayString());
var_dump($url1->toString());
var_dump($url2->toDisplayString());
var_dump($url2->toString());
var_dump($url3->toDisplayString());
var_dump($url3->toString());

?>
--EXPECT--
string(23) "HTTPS://////EXAMPLE.com"
string(23) "https://////EXAMPLE.com"
string(19) "https://example.com"
string(19) "https://example.com"
string(26) "https://example.com/foo/.."
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
