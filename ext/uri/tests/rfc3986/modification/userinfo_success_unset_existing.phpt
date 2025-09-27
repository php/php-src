--TEST--
Test Uri\Rfc3986\Uri component modification - userinfo - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://user:password@example.com");
$uri2 = $uri1->withUserInfo(null);

var_dump($uri1->getRawUserInfo());
var_dump($uri2->getRawUserInfo());
var_dump($uri2->getUserInfo());

?>
--EXPECT--
string(13) "user:password"
NULL
NULL
