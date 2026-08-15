--TEST--
Test Uri\Rfc3986\Uri component retrieval - basic
--FILE--
<?php

function callRfc3986Getters($uri)
{
    var_dump($uri->getScheme());
    var_dump($uri->getRawScheme());
    var_dump($uri->getUsername());
    var_dump($uri->getRawUsername());
    var_dump($uri->getPassword());
    var_dump($uri->getRawPassword());
    var_dump($uri->getUserInfo());
    var_dump($uri->getRawUserInfo());
    var_dump($uri->getHost());
    var_dump($uri->getRawHost());
    var_dump($uri->getPort());
    var_dump($uri->getPath());
    var_dump($uri->getRawPath());
    var_dump($uri->getQuery());
    var_dump($uri->getRawQuery());
    var_dump($uri->getFragment());
    var_dump($uri->getRawFragment());
}

$uri = Uri\Rfc3986\Uri::parse("https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
callRfc3986Getters($uri);

?>
--EXPECT--
string(5) "https"
string(5) "https"
string(8) "username"
string(8) "username"
string(8) "password"
string(8) "password"
string(17) "username:password"
string(17) "username:password"
string(15) "www.example.com"
string(15) "www.example.com"
int(8080)
string(30) "/pathname1/pathname2/pathname3"
string(30) "/pathname1/pathname2/pathname3"
string(10) "query=true"
string(10) "query=true"
string(11) "hash-exists"
string(11) "hash-exists"
