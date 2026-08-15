--TEST--
Test Uri\WhatWg\Url parsing - path - percent-encoded reserved character
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://example.com/foo/bar%2Fbaz");

var_dump($url->toAsciiString());
var_dump($url->getPath());

?>
--EXPECT--
string(33) "https://example.com/foo/bar%2Fbaz"
string(14) "/foo/bar%2Fbaz"
