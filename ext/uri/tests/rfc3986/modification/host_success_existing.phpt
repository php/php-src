--TEST--
Test Uri\Rfc3986\Uri component modification - host - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withHost("test.com");

var_dump($uri1->getRawHost());
var_dump($uri2->getRawHost());
var_dump($uri2->getHost());

?>
--EXPECT--
string(11) "example.com"
string(8) "test.com"
string(8) "test.com"
