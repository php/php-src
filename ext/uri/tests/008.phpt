--TEST--
Test Uri getters
--EXTENSIONS--
uri
--FILE--
<?php

function callGetters(Uri\Uri $uri)
{
    var_dump($uri->getScheme());
    var_dump($uri->getUser());
    var_dump($uri->getPassword());
    var_dump($uri->getHost());
    var_dump($uri->getPort());
    var_dump($uri->getPath());
    var_dump($uri->getQuery());
    var_dump($uri->getFragment());
}

$uri = Uri\Rfc3986\Uri::parse("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
callGetters($uri);

echo "\n";

$uri = Uri\WhatWg\Url::parse("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
callGetters($uri);

?>
--EXPECT--
string(5) "https"
string(8) "username"
string(8) "password"
string(14) "www.google.com"
int(8080)
string(29) "pathname1/pathname2/pathname3"
string(10) "query=true"
string(11) "hash-exists"

string(5) "https"
string(8) "username"
string(8) "password"
string(14) "www.google.com"
int(8080)
string(29) "pathname1/pathname2/pathname3"
string(10) "query=true"
string(11) "hash-exists"
