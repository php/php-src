--TEST--
Test Uri\WhatWg\Url equivalence - returns false - path is not percent-decoded during normalization
--FILE--
<?php

$url1 = new Uri\WhatWg\Url("https://example.com/foo/bar");
$url2 = new Uri\WhatWg\Url("https://example.com/foo/b%61r");

var_dump($url1->equals($url2));
var_dump($url2->equals($url1));

?>
--EXPECT--
bool(false)
bool(false)
