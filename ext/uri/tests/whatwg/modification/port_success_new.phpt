--TEST--
Test Uri\WhatWg\Url component modification - port - adding a new one
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("scheme://example.com");
$url2 = $url1->withPort(443);

var_dump($url1->getPort());
var_dump($url2->getPort());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
int(443)
string(24) "scheme://example.com:443"
