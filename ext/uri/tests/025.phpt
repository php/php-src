--TEST--
Test property mutation - password
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withPassword("pass");
$url3 = $url2->withPassword(null);

var_dump($url1->toString());
var_dump($url2->toString());
var_dump($url3->toString());

?>
--EXPECT--
string(20) "https://example.com/"
string(26) "https://:pass@example.com/"
string(20) "https://example.com/"
