--TEST--
Test property mutation - userinfo
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
var_dump($uri1->getRawUserInfo());
var_dump($uri1->getUserInfo());

$uri2 = $uri1->withUserInfo("user");
var_dump($uri2->getRawUserInfo());
var_dump($uri2->getUserInfo());

$uri3 = $uri2->withUserInfo(null);
var_dump($uri3->getRawUserInfo());
var_dump($uri3->getUserInfo());

$uri4 = $uri3->withUserInfo("%75s%2Fr:pass"); // us/r:pass
var_dump($uri4->getRawUserInfo());
var_dump($uri4->getUserInfo());

$uri1 = Uri\Rfc3986\Uri::parse("/foo");
$uri2 = $uri1->withUserInfo(null);
var_dump($uri2->getPort());

try {
    $uri4->withUserInfo("u:s/r");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

$uri5 = Uri\Rfc3986\Uri::parse("file:///foo/bar/");
$uri6 = $uri5->withUserinfo("user:pass");

var_dump($uri5->getUserInfo());
var_dump($uri6->getUserInfo());

?>
--EXPECT--
NULL
NULL
string(4) "user"
string(4) "user"
NULL
NULL
string(13) "%75s%2Fr:pass"
string(11) "us%2Fr:pass"
NULL
The specified userinfo is malformed
NULL
string(9) "user:pass"
