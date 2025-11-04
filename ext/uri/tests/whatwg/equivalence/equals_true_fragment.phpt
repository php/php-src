--TEST--
Test Uri\WhatWg\Url equivalence - returns true - only fragment differs
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://user:info@example.com:443/foo/bar?abc=123&def=ghi#");
$url2 = Uri\WhatWg\Url::parse("https://user:info@example.com:443/foo/bar?abc=123&def=ghi#hash");

var_dump($url1->equals($url2));
var_dump($url2->equals($url1));

?>
--EXPECT--
bool(true)
bool(true)
