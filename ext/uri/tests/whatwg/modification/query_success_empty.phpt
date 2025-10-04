--TEST--
Test Uri\WhatWg\Url component modification - query - empty string
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withQuery("");

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
NULL
string(20) "https://example.com/"
