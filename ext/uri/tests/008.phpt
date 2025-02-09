--TEST--
Test Uri getters
--EXTENSIONS--
uri
--FILE--
<?php

function callGetters($uri)
{
    var_dump($uri->getScheme());
    if ($uri instanceof Uri\Rfc3986\Uri) {
        var_dump($uri->getRawScheme());
    }
    var_dump($uri->getUser());
    var_dump($uri->getRawUser());
    var_dump($uri->getPassword());
    var_dump($uri->getRawPassword());
    var_dump($uri->getHost());
    if ($uri instanceof Uri\Rfc3986\Uri) {
        var_dump($uri->getRawHost());
    } else {
        var_dump($uri->getHostForDisplay());
    }
    var_dump($uri->getPort());
    var_dump($uri->getPath());
    var_dump($uri->getRawPath());
    var_dump($uri->getQuery());
    var_dump($uri->getRawQuery());
    var_dump($uri->getFragment());
    var_dump($uri->getRawFragment());
}

$uri = Uri\Rfc3986\Uri::parse("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
callGetters($uri);

echo "\n";

$url = Uri\WhatWg\Url::parse("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
callGetters($url);

?>
--EXPECT--
string(5) "https"
string(5) "https"
string(8) "username"
string(8) "username"
string(8) "password"
string(8) "password"
string(14) "www.google.com"
string(14) "www.google.com"
int(8080)
string(30) "/pathname1/pathname2/pathname3"
string(30) "/pathname1/pathname2/pathname3"
string(10) "query=true"
string(10) "query=true"
string(11) "hash-exists"
string(11) "hash-exists"

string(5) "https"
string(8) "username"
string(8) "username"
string(8) "password"
string(8) "password"
string(14) "www.google.com"
string(14) "www.google.com"
int(8080)
string(30) "/pathname1/pathname2/pathname3"
string(30) "/pathname1/pathname2/pathname3"
string(10) "query=true"
string(10) "query=true"
string(11) "hash-exists"
string(11) "hash-exists"
