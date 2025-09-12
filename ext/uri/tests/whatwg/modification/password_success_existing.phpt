--TEST--
Test Uri\WhatWg\Url component modification - password - changing an existing one
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
