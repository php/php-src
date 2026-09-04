--TEST--
Test Uri\WhatWg\Url::withUsername() - success - percent-encoded octet byte
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withUsername("%75s%2Fr");

var_dump($url1->getUsername());

var_dump($url2->getUsername());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
string(8) "%75s%2Fr"
string(29) "https://%75s%2Fr@example.com/"
