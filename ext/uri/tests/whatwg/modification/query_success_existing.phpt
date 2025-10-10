--TEST--
Test Uri\WhatWg\Url component modification - query - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com?foo=bar");
$url2 = $url1->withQuery("foo=bar&baz=qux");

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(7) "foo=bar"
string(15) "foo=bar&baz=qux"
string(36) "https://example.com/?foo=bar&baz=qux"
