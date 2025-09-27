--TEST--
Test Uri\WhatWg\Url component modification - query - with leading question mark
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com/foo/bar");
$url2 = $url1->withQuery("?foo=bar");

var_dump($url1->getQuery());
var_dump($url2->getQuery());

?>
--EXPECT--
NULL
string(7) "foo=bar"
