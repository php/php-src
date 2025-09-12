--TEST--
Test Uri\WhatWg\Url component modification - password - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withPassword("p%61ssword");

var_dump($url1->getPassword());
var_dump($url2->getPassword());

?>
--EXPECT--
NULL
string(10) "p%61ssword"
