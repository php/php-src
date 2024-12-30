--TEST--
Test cloning error
--EXTENSIONS--
uri
--FILE--
<?php

readonly class MyRfc3986Uri extends Uri\Rfc3986\Uri
{
    public function __construct() {
        // Parent constructor isn't called
    }
}

readonly class MyWhatWgUri extends Uri\WhatWg\Url
{
    public function __construct() {
        // Parent constructor isn't called
    }
}

$uri1 = new MyRfc3986Uri("https://example.com");
try {
    $uri2 = clone $uri1;
} catch (Uri\UninitializedUriException $e) {
    echo $e->getMessage() . "\n";
}
var_dump($uri1);

$uri3 = new MyWhatWgUri("https://example.com");
try {
    $uri4 = clone $uri3;
} catch (Uri\UninitializedUriException $e) {
    echo $e->getMessage() . "\n";
}
var_dump($uri3);

?>
--EXPECTF--
MyRfc3986Uri object is not correctly initialized
object(MyRfc3986Uri)#1 (%d) {
}
MyWhatWgUri object is not correctly initialized
object(MyWhatWgUri)#2 (%d) {
}
