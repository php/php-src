--TEST--
Test Uri\Rfc3986\Uri component modification - userinfo - adding a new one
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withUserInfo("user:password");

var_dump($uri1->getRawUserInfo());
var_dump($uri2->getRawUserInfo());
var_dump($uri2->toRawString());
var_dump($uri2->getUserInfo());
var_dump($uri2->toString());

?>
--EXPECT--
NULL
string(13) "user:password"
string(33) "https://user:password@example.com"
string(13) "user:password"
string(33) "https://user:password@example.com"
