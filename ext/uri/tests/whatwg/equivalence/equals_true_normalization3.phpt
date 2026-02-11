--TEST--
Test Uri\WhatWg\Url equivalence - returns true - after empty path normalization
--FILE--
<?php

$url1 = new Uri\WhatWg\Url("https://example.com");
$url2 = new Uri\WhatWg\Url("https://example.com/");

var_dump($url1->equals($url2));
var_dump($url2->equals($url1));

?>
--EXPECT--
bool(true)
bool(true)
