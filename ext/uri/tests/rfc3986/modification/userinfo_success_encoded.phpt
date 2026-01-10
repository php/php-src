--TEST--
Test Uri\Rfc3986\Uri component modification - userinfo - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withUserInfo("%75s%2Fr:password"); // us/r:password

var_dump($uri1->getRawUserInfo());
var_dump($uri2->getRawUserInfo());
var_dump($uri2->toRawString());
var_dump($uri2->getUserInfo());
var_dump($uri2->toString());

?>
--EXPECT--
NULL
string(17) "%75s%2Fr:password"
string(37) "https://%75s%2Fr:password@example.com"
string(15) "us%2Fr:password"
string(35) "https://us%2Fr:password@example.com"
