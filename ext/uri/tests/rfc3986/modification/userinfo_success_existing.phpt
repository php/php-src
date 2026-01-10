--TEST--
Test Uri\Rfc3986\Uri component modification - userinfo - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://:pass@example.com");
$uri2 = $uri1->withUserInfo("user:password");

var_dump($uri1->getRawUserInfo());
var_dump($uri2->getRawUserInfo());
var_dump($uri2->toRawString());
var_dump($uri2->getUserInfo());
var_dump($uri2->toString());

?>
--EXPECT--
string(5) ":pass"
string(13) "user:password"
string(33) "https://user:password@example.com"
string(13) "user:password"
string(33) "https://user:password@example.com"
