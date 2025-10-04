--TEST--
Test Uri\WhatWg\Url component modification - password - characters from the percent encode set
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://user:password@example.com");
$url2 = $url1->withPassword("p:s/");

var_dump($url1->getPassword());
var_dump($url2->getPassword());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(8) "password"
string(8) "p%3As%2F"
string(34) "https://user:p%3As%2F@example.com/"
