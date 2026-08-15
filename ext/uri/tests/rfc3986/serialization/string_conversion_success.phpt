--TEST--
Test Uri\Rfc3986\Uri string conversion
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("HTTPS://////EXAMPLE.com");
$uri2 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri3 = Uri\Rfc3986\Uri::parse("https://example.com/foo/..");
var_dump($uri1->toRawString());
var_dump($uri1->toString());
var_dump($uri2->toRawString());
var_dump($uri2->toString());
var_dump($uri3->toRawString());
var_dump($uri3->toString());

?>
--EXPECT--
string(23) "HTTPS://////EXAMPLE.com"
string(23) "https://////EXAMPLE.com"
string(19) "https://example.com"
string(19) "https://example.com"
string(26) "https://example.com/foo/.."
string(20) "https://example.com/"
