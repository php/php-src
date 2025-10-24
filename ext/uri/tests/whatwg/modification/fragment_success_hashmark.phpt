--TEST--
Test Uri\WhatWg\Url component modification - fragment -only a hashmark character
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withFragment("#");

var_dump($url1->getFragment());
var_dump($url2->getFragment());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
NULL
string(21) "https://example.com/#"
