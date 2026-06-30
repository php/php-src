--TEST--
Test Uri\WhatWg\Url component modification - path - unicode characters
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withPath("/ő");

var_dump($url1->getPath());
var_dump($url2->getPath());
var_dump($url2->toAsciiString());
?>
--EXPECT--
string(1) "/"
string(7) "/%C5%91"
string(26) "https://example.com/%C5%91"
