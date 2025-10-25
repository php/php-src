--TEST--
Test Uri\WhatWg\Url component modification - host - IDNA characters
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withHost("éxämple.com");

var_dump($url1->getAsciiHost());
var_dump($url2->getAsciiHost());
var_dump($url2->toAsciiString());
var_dump($url2->getUnicodeHost());
var_dump($url2->toUnicodeString());

?>
--EXPECT--
string(11) "example.com"
string(19) "xn--xmple-gra7a.com"
string(28) "https://xn--xmple-gra7a.com/"
string(13) "éxämple.com"
string(22) "https://éxämple.com/"
