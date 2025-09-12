--TEST--
Test Uri\WhatWg\Url component modification - username - changing existing
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withUsername("username");

var_dump($url1->getUsername());
var_dump($url2->getUsername());

?>
--EXPECT--
NULL
string(8) "username"
