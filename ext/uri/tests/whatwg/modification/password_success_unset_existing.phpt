--TEST--
Test Uri\WhatWg\Url component modification - username - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://username:password@example.com");
$url2 = $url1->withPassword(null);

var_dump($url1->getPassword());
var_dump($url2->getPassword());

?>
--EXPECT--
string(8) "password"
NULL
