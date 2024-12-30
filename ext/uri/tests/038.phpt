--TEST--
Test toNormalizedString()
--EXTENSIONS--
uri
--XFAIL--
Cloning Rfc3986Uris doesn't copy the path properly yet
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("HTTPS://////EXAMPLE.com");
$uri2 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri3 = Uri\Rfc3986\Uri::parse("https://example.com/foo/..");

var_dump($uri1->toNormalizedString());
var_dump($uri1->toString());
var_dump($uri2->toNormalizedString());
var_dump($uri2->toString());
var_dump($uri3->toNormalizedString());
var_dump($uri3->toString());

$uri1 = Uri\WhatWg\Url::parse("HTTPS://////EXAMPLE.com");
$uri2 = Uri\WhatWg\Url::parse("https://example.com");
$uri3 = Uri\WhatWg\Url::parse("https://example.com/foo/..");

var_dump($uri1->toNormalizedString());
var_dump($uri1->toString());
var_dump($uri2->toNormalizedString());
var_dump($uri2->toString());
var_dump($uri3->toNormalizedString());
var_dump($uri3->toString());

?>
--EXPECT--
string(23) "HTTPS://////EXAMPLE.com"
string(19) "https://example.com"
string(19) "https://example.com"
string(19) "https://example.com"
string(19) "https://example.com"
string(26) "https://example.com/foo/.."
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
