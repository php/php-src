--TEST--
Test percent-encoding of different URI components
--EXTENSIONS--
uri
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

$uri = Uri\Rfc3986\Uri::parse("http://%61pple:p%61ss@ex%61mple.com/foob%61r?%61bc=%61bc#%61bc");
callRfc3986Getters($uri);

echo "\n";

$url = Uri\WhatWg\Url::parse("http://%61pple:p%61ss@ex%61mple.com/foob%61r?%61bc=%61bc#%61bc");
callWhatWgGetters($url);

?>
--EXPECT--
string(4) "http"
string(4) "http"
string(5) "apple"
string(7) "%61pple"
string(4) "pass"
string(6) "p%61ss"
string(10) "apple:pass"
string(14) "%61pple:p%61ss"
string(11) "example.com"
string(13) "ex%61mple.com"
NULL
string(7) "/foobar"
string(9) "/foob%61r"
string(7) "abc=abc"
string(11) "%61bc=%61bc"
string(3) "abc"
string(5) "%61bc"

string(4) "http"
string(7) "%61pple"
string(6) "p%61ss"
string(11) "example.com"
string(11) "example.com"
NULL
string(9) "/foob%61r"
string(11) "%61bc=%61bc"
string(5) "%61bc"
