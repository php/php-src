--TEST--
Test property mutation - user
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withUser("user");
$url3 = $url2->withUser(null);
$url4 = $url3->withUser("%75s%2Fr"); // us/r
$url5 = $url4->withUser("u:s/r");

$url6 = Uri\WhatWg\Url::parse("file:///foo/bar/");
$url6 = $url6->withUser("user");

var_dump($url2->getRawUser());
var_dump($url3->getRawUser());
var_dump($url4->getRawUser());
var_dump($url4->getUser());
var_dump($url5->getRawUser());
var_dump($url5->getUser());
var_dump($url6->getRawUser());

?>
--EXPECT--
string(4) "user"
NULL
string(8) "%75s%2Fr"
string(4) "us/r"
string(9) "u%3As%2Fr"
string(5) "u:s/r"
NULL
