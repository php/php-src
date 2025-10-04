--TEST--
Test Uri\WhatWg\Url component modification - username - adding a new one
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withUsername("username");

var_dump($url1->getUsername());
var_dump($url2->getUsername());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
string(8) "username"
string(29) "https://username@example.com/"
