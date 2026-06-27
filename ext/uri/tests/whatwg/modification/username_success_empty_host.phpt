--TEST--
Test Uri\WhatWg\Url component modification - username - empty host
--FILE--
<?php

$url1 = new Uri\WhatWg\Url("scheme://");
$url2 = $url1->withUsername("user");

var_dump($url1->getUsername());
var_dump($url2->getUsername());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
NULL
string(9) "scheme://"
