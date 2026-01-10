--TEST--
Test property mutation - password
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withPassword("pass");
$url3 = $url2->withPassword(null);
$url4 = $url3->withPassword("p%61ss");
$url5 = $url4->withPassword("p:s/");

$url6 = Uri\WhatWg\Url::parse("file:///foo/bar/");
$url6 = $url6->withUsername("pass");

var_dump($url2->getPassword());
var_dump($url3->getPassword());
var_dump($url4->getPassword());
var_dump($url5->getPassword());
var_dump($url6->getPassword());

?>
--EXPECT--
string(4) "pass"
NULL
string(6) "p%61ss"
string(8) "p%3As%2F"
NULL
