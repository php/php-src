--TEST--
Test Uri\WhatWg\Url component modification - username - URL encoded characters
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withUsername("%75s%2Fr");

var_dump($url2->getUsername());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(8) "%75s%2Fr"
string(29) "https://%75s%2Fr@example.com/"
