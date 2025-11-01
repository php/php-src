--TEST--
Test Uri\WhatWg\Url component modification - username - unsetting non-existent
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://:password@example.com");
$url2 = $url1->withUsername(null);

var_dump($url1->getUsername());
var_dump($url2->getUsername());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(0) ""
string(0) ""
string(30) "https://:password@example.com/"
