--TEST--
Test Uri\WhatWg\Url component modification - password - empty string
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://@example.com");
$url2 = $url1->withPassword("");

var_dump($url1->getPassword());
var_dump($url2->getPassword());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
NULL
string(20) "https://example.com/"
