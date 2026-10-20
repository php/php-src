--TEST--
Test Uri\WhatWg\Url::withPort() - success - file scheme
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("file:///foo/bar");
$url2 = $url1->withPort(80);

var_dump($url1->getPort());

var_dump($url2->getPort());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
NULL
string(15) "file:///foo/bar"
