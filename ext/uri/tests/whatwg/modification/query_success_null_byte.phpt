--TEST--
Test Uri\WhatWg\Url component modification - query - null byte
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withQuery("f\0o=bar&baz=q\0x");

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
string(19) "f%00o=bar&baz=q%00x"
string(40) "https://example.com/?f%00o=bar&baz=q%00x"
