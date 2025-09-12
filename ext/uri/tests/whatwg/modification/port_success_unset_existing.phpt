--TEST--
Test Uri\WhatWg\Url component modification - port - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com:80");
$url2 = $url1->withPort(null);

var_dump($url1->getPort());
var_dump($url2->getPort());

?>
--EXPECT--
int(80)
NULL
