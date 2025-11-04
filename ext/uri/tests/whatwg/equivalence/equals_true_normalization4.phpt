--TEST--
Test Uri\WhatWg\Url equivalence - returns true - after host normalization
--FILE--
<?php

$url1 = new Uri\WhatWg\Url("https://example%2ecom:443");
$url2 = new Uri\WhatWg\Url("https://example.com:0443");

var_dump($url1->equals($url2));
var_dump($url2->equals($url1));

?>
--EXPECT--
bool(true)
bool(true)
