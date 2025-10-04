--TEST--
Test Uri\WhatWg\Url component modification - query - unicode characters
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withQuery("ő");

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
string(6) "%C5%91"
string(27) "https://example.com/?%C5%91"
