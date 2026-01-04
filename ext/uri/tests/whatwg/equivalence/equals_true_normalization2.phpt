--TEST--
Test Uri\WhatWg\Url equivalence - returns true - after IPv6 normalization
--FILE--
<?php

$url1 = new Uri\WhatWg\Url("https://[2001:0db8:0001:0000:0000:0ab9:C0A8:0102]");
$url2 = new Uri\WhatWg\Url("https://[2001:DB8:1::AB9:C0A8:102]");

var_dump($url1->equals($url2));
var_dump($url2->equals($url1));

?>
--EXPECT--
bool(true)
bool(true)
