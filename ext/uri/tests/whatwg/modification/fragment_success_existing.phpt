--TEST--
Test Uri\WhatWg\Url component modification - fragment - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com#foo");
$url2 = $url1->withFragment("bar");

var_dump($url1->getFragment());
var_dump($url2->getFragment());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(3) "foo"
string(3) "bar"
string(24) "https://example.com/#bar"
