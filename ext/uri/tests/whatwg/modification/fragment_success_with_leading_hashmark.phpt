--TEST--
Test Uri\WhatWg\Url component modification - fragment - with leading hashmark
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withFragment("#fragment");

var_dump($url1->getFragment());
var_dump($url2->getFragment());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
string(8) "fragment"
string(29) "https://example.com/#fragment"
