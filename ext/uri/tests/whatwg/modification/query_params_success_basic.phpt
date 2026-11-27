--TEST--
Test Uri\WhatWg\Url::withQueryParams() - success - replace a non-empty query
--FILE--
<?php

$url1 = new Uri\WhatWg\Url("https://example.com?old=1");

$params = Uri\QueryParams::parseWhatWgUrlSearchParams("foo=bar&foo=baz");

$url2 = $url1->withQueryParams($params);

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(5) "old=1"
string(15) "foo=bar&foo=baz"
string(36) "https://example.com/?foo=bar&foo=baz"
