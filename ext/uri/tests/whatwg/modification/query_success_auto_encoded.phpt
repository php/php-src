--TEST--
Test Uri\WhatWg\Url component modification - query - characters from the percent encode set
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withQuery("#foo ");

var_dump($url1->getQuery());
var_dump($url2->getQuery());

?>
--EXPECT--
NULL
string(9) "%23foo%20"
