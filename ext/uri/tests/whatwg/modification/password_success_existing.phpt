--TEST--
Test Uri\WhatWg\Url component modification - password - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://:pass@example.com");
$url2 = $url1->withPassword("password");

var_dump($url1->getPassword());
var_dump($url2->getPassword());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(4) "pass"
string(8) "password"
string(30) "https://:password@example.com/"
