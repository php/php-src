--TEST--
Test Uri\WhatWg\Url component modification - fragment - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com#foo");
$url2 = $url1->withFragment(null);

var_dump($url1->getFragment());
var_dump($url2->getFragment());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(3) "foo"
NULL
string(20) "https://example.com/"
