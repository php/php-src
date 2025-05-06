--TEST--
Test property mutation - username
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withUsername("user");
$url3 = $url2->withUsername(null);
$url4 = $url3->withUsername("%75s%2Fr"); // us/r
$url5 = $url4->withUsername("u:s/r");

$url6 = Uri\WhatWg\Url::parse("file:///foo/bar/");
$url6 = $url6->withUsername("user");

var_dump($url2->getUsername());
var_dump($url3->getUsername());
var_dump($url4->getUsername());
var_dump($url5->getUsername());
var_dump($url6->getUsername());

?>
--EXPECT--
string(4) "user"
NULL
string(8) "%75s%2Fr"
string(9) "u%3As%2Fr"
NULL
