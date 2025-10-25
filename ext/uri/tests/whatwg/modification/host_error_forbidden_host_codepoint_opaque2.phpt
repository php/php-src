--TEST--
Test Uri\WhatWg\Url component modification - host - forbidden host code point
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("foo://example.com");
$url2 = $url1->withHost("ex#mple.com");

var_dump($url1->getAsciiHost());
var_dump($url2->getAsciiHost());

?>
--EXPECT--
string(11) "example.com"
string(2) "ex"
