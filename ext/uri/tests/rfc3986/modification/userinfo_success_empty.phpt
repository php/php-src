--TEST--
Test Uri\Rfc3986\Uri::withUserInfo() - success - empty string
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withUserInfo("");

var_dump($uri1->getRawUserInfo());
var_dump($uri2->getRawUserInfo());
var_dump($uri2->getUserInfo());

?>
--EXPECT--
NULL
string(0) ""
string(0) ""
