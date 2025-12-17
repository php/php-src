--TEST--
Test Uri\WhatWg\Url component modification - port - unsetting non-existent
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("ftp://example.com");
$url2 = $url1->withPort(null);

var_dump($url2->getPort());
var_dump($url2->getPort());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
NULL
string(18) "ftp://example.com/"
