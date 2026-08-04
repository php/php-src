--TEST--
Test Uri\WhatWg\Url component modification - port - empty host
--FILE--
<?php

$url1 = new Uri\WhatWg\Url("scheme://");
$url2 = $url1->withPort(433);

var_dump($url1->getPort());
var_dump($url2->getPort());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
NULL
string(9) "scheme://"
