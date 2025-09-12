--TEST--
Test Uri\WhatWg\Url component modification - username - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://username:password@example.com");
$url2 = $url1->withUsername(null);

var_dump($url1->getUsername());
var_dump($url2->getUsername());

?>
--EXPECT--
string(8) "username"
NULL
