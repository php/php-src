--TEST--
Test Uri\WhatWg\Url component modification - username - characters from the percent encode set
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://user:password@example.com");
$url2 = $url1->withUsername("u:s/r");

var_dump($url1->getUsername());
var_dump($url2->getUsername());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(4) "user"
string(9) "u%3As%2Fr"
string(39) "https://u%3As%2Fr:password@example.com/"
