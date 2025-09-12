--TEST--
Test Uri\WhatWg\Url component modification - query - unsetting not-existent
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withQuery(null);

var_dump($url1->getQuery());
var_dump($url2->getQuery());

?>
--EXPECT--
NULL
NULL
