--TEST--
Test Uri\WhatWg\Url component modification - query - only a question mark character
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withQuery("?");

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
string(0) ""
string(21) "https://example.com/?"
