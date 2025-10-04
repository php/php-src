--TEST--
Test Uri\Rfc3986\Uri component modification - userinfo - unsetting non-existent
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("/foo/bar");
$uri2 = $uri1->withUserInfo(null);

var_dump($uri1->getRawUserInfo());
var_dump($uri2->getRawUserInfo());
var_dump($uri2->toRawString());
var_dump($uri2->getUserInfo());
var_dump($uri2->toString());

?>
--EXPECT--
NULL
NULL
string(8) "/foo/bar"
NULL
string(8) "/foo/bar"
