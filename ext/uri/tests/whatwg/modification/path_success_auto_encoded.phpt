--TEST--
Test Uri\WhatWg\Url component modification - path - characters from the percent encode set
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com/");
$url2 = $url1->withPath("/p^th#");

var_dump($url1->getPath());
var_dump($url2->getPath());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(1) "/"
string(10) "/p%5Eth%23"
string(29) "https://example.com/p%5Eth%23"
