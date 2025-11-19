--TEST--
Test Uri\WhatWg\Url component modification - fragment - null byte
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withFragment("frag\0ment");

var_dump($url1->getFragment());
var_dump($url2->getFragment());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
string(11) "frag%00ment"
string(32) "https://example.com/#frag%00ment"
