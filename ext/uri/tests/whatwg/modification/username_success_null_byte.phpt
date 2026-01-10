--TEST--
Test Uri\WhatWg\Url component modification - username - null byte
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withUsername("usern\0me");

var_dump($url1->getUsername());
var_dump($url2->getUsername());
var_dump($url2->toAsciiString());


?>
--EXPECT--
NULL
string(10) "usern%00me"
string(31) "https://usern%00me@example.com/"
