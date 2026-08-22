--TEST--
Test Uri\WhatWg\Url reference resolution - resolve() - success
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://example.com");

var_dump($url->resolve("/foo/")->toAsciiString());
var_dump($url->resolve("https://test.com/foo")->toAsciiString());

?>
--EXPECT--
string(24) "https://example.com/foo/"
string(20) "https://test.com/foo"
