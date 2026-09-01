--TEST--
Test Uri\Rfc3986\Uri component modification - host - URL encoded reserved characters
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withHost("t%3As%2Ft.com");

var_dump($uri1->getRawHost());

var_dump($uri2->getRawHost());
var_dump($uri2->toRawString());
var_dump($uri2->getHost());
var_dump($uri2->toString());

?>
--EXPECT--
string(11) "example.com"
string(13) "t%3As%2Ft.com"
string(21) "https://t%3As%2Ft.com"
string(13) "t%3As%2Ft.com"
string(21) "https://t%3As%2Ft.com"
