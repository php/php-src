--TEST--
Test Uri\Rfc3986\Uri component modification - host - IP future address
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withHost("[vF.addr]");

var_dump($uri1->getRawHost());
var_dump($uri2->getRawHost());
var_dump($uri2->getHost());

?>
--EXPECT--
string(11) "example.com"
string(9) "[vF.addr]"
string(9) "[vf.addr]"
