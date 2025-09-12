--TEST--
Test Uri\WhatWg\Url component modification - path - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withPath("/foo%2Fbar"); // /foo/bar

var_dump($url1->getPath());
var_dump($url2->getPath());

?>
--EXPECT--
string(1) "/"
string(10) "/foo%2Fbar"
