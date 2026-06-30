--TEST--
Test Uri\WhatWg\Url component modification - password - empty host
--FILE--
<?php

$url1 = new Uri\WhatWg\Url("scheme://");
$url2 = $url1->withPassword("password");

var_dump($url1->getPassword());
var_dump($url2->getPassword());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
NULL
string(9) "scheme://"
