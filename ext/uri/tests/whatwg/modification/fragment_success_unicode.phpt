--TEST--
Test Uri\WhatWg\Url component modification - fragment - unicode characters
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withFragment("ő");

var_dump($url1->getFragment());
var_dump($url2->getFragment());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
string(6) "%C5%91"
string(27) "https://example.com/#%C5%91"
