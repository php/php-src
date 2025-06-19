--TEST--
Test percent-encoding of different URI components
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

$url = Uri\WhatWg\Url::parse("http://%61pple:p%61ss@ex%61mple.com/foob%61r?%61bc=%61bc#%61bc");
callWhatWgGetters($url);

?>
--EXPECT--
string(4) "http"
string(7) "%61pple"
string(6) "p%61ss"
string(11) "example.com"
string(11) "example.com"
NULL
string(9) "/foob%61r"
string(11) "%61bc=%61bc"
string(5) "%61bc"
