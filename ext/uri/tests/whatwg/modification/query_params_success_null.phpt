--TEST--
Test Uri\WhatWg\Url::withQueryParams() - success - null removes a non-empty query
--FILE--
<?php

$url1 = new Uri\WhatWg\Url("https://example.com?foo=bar");

$url2 = $url1->withQueryParams(null);

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(7) "foo=bar"
NULL
string(20) "https://example.com/"
