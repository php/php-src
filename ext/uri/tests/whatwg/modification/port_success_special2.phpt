--TEST--
Test Uri\WhatWg\Url component modification - port - adding a new one for a special URL
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withPort(443);

var_dump($url1->getPort());
var_dump($url2->getPort());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
NULL
string(20) "https://example.com/"
