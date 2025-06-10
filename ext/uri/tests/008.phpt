--TEST--
Test Uri getters
--EXTENSIONS--
uri
--FILE--
<?php

function callWhatWgGetters($url)
{
    var_dump($url->getScheme());
    var_dump($url->getUsername());
    var_dump($url->getPassword());
    var_dump($url->getAsciiHost());
    var_dump($url->getUnicodeHost());
    var_dump($url->getPort());
    var_dump($url->getPath());
    var_dump($url->getQuery());
    var_dump($url->getFragment());
}

$url = Uri\WhatWg\Url::parse("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
callWhatWgGetters($url);

?>
--EXPECT--
string(5) "https"
string(8) "username"
string(8) "password"
string(14) "www.google.com"
string(14) "www.google.com"
int(8080)
string(30) "/pathname1/pathname2/pathname3"
string(10) "query=true"
string(11) "hash-exists"
