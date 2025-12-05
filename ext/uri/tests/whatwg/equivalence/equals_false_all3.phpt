--TEST--
Test Uri\WhatWg\Url equivalence - returns false - all components
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://user:info@example.com:443/foo/bar?abc=123&def=ghi#hashmark");
$url2 = Uri\WhatWg\Url::parse("http://user@example.com:443/foo/bar/baz?abc=123&def=ghi#hashmark");

var_dump($url1->equals($url2));
var_dump($url2->equals($url1));

?>
--EXPECT--
bool(false)
bool(false)
