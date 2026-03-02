--TEST--
Test Uri\WhatWg\Url component modification - path - characters from the percent encode set
--EXTENSIONS--
uri
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
string(8) "/p^th%23"
string(27) "https://example.com/p^th%23"
