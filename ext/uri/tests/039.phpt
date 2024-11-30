--TEST--
Test percent-encoding of different URI components
--EXTENSIONS--
uri
--XFAIL--
Cloning Rfc3986Uris doesn't copy the path properly yet
--FILE--
<?php

function printUri(Uri\Uri $uri) {
    var_dump($uri->getUser());
    var_dump($uri->getPassword());
    var_dump($uri->getHost());
    var_dump($uri->getPath());
    var_dump($uri->getQuery());
    var_dump($uri->getFragment());
}

$uri = Uri\Rfc3986Uri::parse("http://%61pple:p%61ss@ex%61mple.com/foob%61r?%61bc=%61bc#%61bc");
printUri($uri);
$uri = $uri->normalize();
printUri($uri);

$uri = Uri\WhatWgUri::parse("http://%61pple:p%61ss@ex%61mple.com/foob%61r?%61bc=%61bc#%61bc");
printUri($uri);
$uri = $uri->normalize();
printUri($uri);

?>
--EXPECT--
string(7) "%61pple"
string(6) "p%61ss"
string(13) "ex%61mple.com"
string(8) "foob%61r"
string(11) "%61bc=%61bc"
string(5) "%61bc"
string(5) "apple"
string(4) "pass"
string(11) "example.com"
string(6) "foobar"
string(11) "abc=abc"
string(5) "abc"
string(7) "%61pple"
string(6) "p%61ss"
string(11) "example.com"
string(8) "foob%61r"
string(11) "%61bc=%61bc"
string(5) "%61bc"
string(7) "%61pple"
string(6) "p%61ss"
string(11) "example.com"
string(8) "foob%61r"
string(11) "%61bc=%61bc"
string(5) "%61bc"
