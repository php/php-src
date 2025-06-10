--TEST--
Test toString()
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("HTTPS://////EXAMPLE.com");
$url2 = Uri\WhatWg\Url::parse("https://example.com");
$url3 = Uri\WhatWg\Url::parse("https://example.com/foo/..");

var_dump($url1->toUnicodeString());
var_dump($url1->toAsciiString());
var_dump($url2->toUnicodeString());
var_dump($url2->toAsciiString());
var_dump($url3->toUnicodeString());
var_dump($url3->toAsciiString());

?>
--EXPECT--
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
