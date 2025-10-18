--TEST--
Test Uri\WhatWg\Url component modification - host - forbidden host codepoint
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withHost("ex#mple.com");

var_dump($url1->getAsciiHost());
var_dump($url2->getAsciiHost());

?>
--EXPECT--
string(11) "example.com"
string(2) "ex"
