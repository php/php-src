--TEST--
Test property mutation - userinfo
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withUserInfo("user");
$uri3 = $uri2->withUserInfo(null);
$uri4 = $uri3->withUserInfo("%75s%2Fr:pass"); // us/r:pass

var_dump($uri1->getUserInfo());
var_dump($uri2->getUserInfo());
var_dump($uri3->getUserInfo());
var_dump($uri4->getUserInfo());
var_dump($uri4->getRawUserInfo());

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
string(4) "user"
NULL
string(11) "us%2Fr:pass"
string(13) "%75s%2Fr:pass"
NULL
The specified userinfo is malformed
NULL
string(9) "user:pass"
