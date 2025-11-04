--TEST--
Test Uri\WhatWg\Url component modification - password - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://username:password@example.com");
$url2 = $url1->withPassword(null);

var_dump($url1->getPassword());
var_dump($url2->getPassword());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(8) "password"
string(0) ""
string(29) "https://username@example.com/"
