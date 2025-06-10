--TEST--
Test resolve() method - success cases
--EXTENSIONS--
uri
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://example.com");

var_dump($url->resolve("/foo/")->toAsciiString());
var_dump($url->resolve("https://test.com/foo")->toAsciiString());

?>
--EXPECTF--
string(24) "https://example.com/foo/"
string(20) "https://test.com/foo"
