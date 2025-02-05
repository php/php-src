--TEST--
Test property mutation - user
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withUser("user");
$url3 = $url2->withUser(null);

var_dump($url1->toString());
var_dump($url2->toString());
var_dump($url3->toString());

?>
--EXPECT--
string(20) "https://example.com/"
string(25) "https://user@example.com/"
string(20) "https://example.com/"
