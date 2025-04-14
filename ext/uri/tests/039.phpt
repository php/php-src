--TEST--
Test percent-encoding of different URI components
--EXTENSIONS--
uri
--FILE--
<?php

function printUri($uri) {
    var_dump($uri->getUsername());
    var_dump($uri->getRawUsername());
    var_dump($uri->getPassword());
    var_dump($uri->getRawPassword());
    if ($uri instanceof Uri\Rfc3986\Uri) {
        var_dump($uri->getHost());
        var_dump($uri->getRawHost());
    } else {
        var_dump($uri->getAsciiHost());
        var_dump($uri->getUnicodeHost());
    }
    var_dump($uri->getPath());
    var_dump($uri->getRawPath());
    var_dump($uri->getQuery());
    var_dump($uri->getRawQuery());
    var_dump($uri->getFragment());
    var_dump($uri->getRawFragment());
}

$uri = Uri\Rfc3986\Uri::parse("http://%61pple:p%61ss@ex%61mple.com/foob%61r?%61bc=%61bc#%61bc");
printUri($uri);

$url = Uri\WhatWg\Url::parse("http://%61pple:p%61ss@ex%61mple.com/foob%61r?%61bc=%61bc#%61bc");
printUri($url);

?>
--EXPECT--
string(5) "apple"
string(7) "%61pple"
string(4) "pass"
string(6) "p%61ss"
string(11) "example.com"
string(13) "ex%61mple.com"
string(7) "/foobar"
string(9) "/foob%61r"
string(7) "abc=abc"
string(11) "%61bc=%61bc"
string(3) "abc"
string(5) "%61bc"
string(5) "apple"
string(7) "%61pple"
string(4) "pass"
string(6) "p%61ss"
string(11) "example.com"
string(11) "example.com"
string(7) "/foobar"
string(9) "/foob%61r"
string(7) "abc=abc"
string(11) "%61bc=%61bc"
string(3) "abc"
string(5) "%61bc"
