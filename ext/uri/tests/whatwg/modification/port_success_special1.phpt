--TEST--
Test Uri\WhatWg\Url component modification - port - adding a new one for a special URL
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("http://example.com:88");
$url2 = $url1->withPort(80);

var_dump($url1->getPort());
var_dump($url2->getPort());
var_dump($url2->toAsciiString());

?>
--EXPECT--
int(88)
NULL
string(19) "http://example.com/"
