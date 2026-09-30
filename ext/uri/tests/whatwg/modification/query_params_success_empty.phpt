--TEST--
Test Uri\WhatWg\Url::withQueryParams() - success - empty list with an absent query
--FILE--
<?php

$url1 = new Uri\WhatWg\Url("https://example.com");

$params = new Uri\QueryParams();

$url2 = $url1->withQueryParams($params);

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
NULL
string(20) "https://example.com/"
