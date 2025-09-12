--TEST--
Test Uri\Rfc3986\Uri omponent modification - userinfo - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withUserInfo("user:password");

var_dump($uri1->getRawUserInfo());
var_dump($uri2->getRawUserInfo());
var_dump($uri2->getUserInfo());

?>
--EXPECT--
NULL
string(13) "user:password"
string(13) "user:password"
