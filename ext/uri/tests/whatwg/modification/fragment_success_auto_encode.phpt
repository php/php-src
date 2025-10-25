--TEST--
Test Uri\WhatWg\Url component modification - fragment - characters from the percent encode set
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withFragment("<>");

var_dump($url1->getFragment());
var_dump($url2->getFragment());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
string(6) "%3C%3E"
string(27) "https://example.com/#%3C%3E"
