--TEST--
Test Uri\WhatWg\Url component modification - query - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com?foo=bar");
$url2 = $url1->withQuery(null);

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(7) "foo=bar"
NULL
string(20) "https://example.com/"
