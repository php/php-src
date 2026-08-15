--TEST--
Test Uri\WhatWg\Url component retrieval - basic
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");

var_dump($url->getScheme());
var_dump($url->getUsername());
var_dump($url->getPassword());
var_dump($url->getAsciiHost());
var_dump($url->getUnicodeHost());
var_dump($url->getPort());
var_dump($url->getPath());
var_dump($url->getQuery());
var_dump($url->getFragment());

?>
--EXPECT--
string(5) "https"
string(8) "username"
string(8) "password"
string(15) "www.example.com"
string(15) "www.example.com"
int(8080)
string(30) "/pathname1/pathname2/pathname3"
string(10) "query=true"
string(11) "hash-exists"
