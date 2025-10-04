--TEST--
Test Uri\WhatWg\Url component modification - fragment - unsetting not-existent
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withFragment(null);

var_dump($url1->getFragment());
var_dump($url2->getFragment());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
NULL
string(20) "https://example.com/"
