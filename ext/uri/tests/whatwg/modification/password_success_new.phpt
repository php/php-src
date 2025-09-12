--TEST--
Test Uri\WhatWg\Url component modification - username - adding a new one
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withPassword("password");

var_dump($url1->getPassword());
var_dump($url2->getPassword());

?>
--EXPECT--
NULL
string(8) "password"
