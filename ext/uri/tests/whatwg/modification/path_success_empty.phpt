--TEST--
Test Uri\WhatWg\Url component modification - path - empty string
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withPath("");

var_dump($url1->getPath());
var_dump($url2->getPath());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(1) "/"
string(1) "/"
string(20) "https://example.com/"
