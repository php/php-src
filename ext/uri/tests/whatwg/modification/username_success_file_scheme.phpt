--TEST--
Test Uri\WhatWg\Url component modification - username - file scheme
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("file:///foo/bar/");
$url2 = $url1->withUsername("user");

var_dump($url1->getUsername());

var_dump($url2->getUsername());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
NULL
string(16) "file:///foo/bar/"
