--TEST--
Test Uri\WhatWg\Url component modification - password - null byte
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withPassword("pass\0word");

var_dump($url1->getPassword());
var_dump($url2->getPassword());
var_dump($url2->toAsciiString());


?>
--EXPECT--
NULL
string(11) "pass%00word"
string(33) "https://:pass%00word@example.com/"
