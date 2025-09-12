--TEST--
Test Uri\WhatWg\Url component modification - fragment - characters from the percent encode set
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\WhatWg\Url::parse("https://example.com");
$uri2 = $uri1->withFragment("<>");

var_dump($uri1->getFragment());
var_dump($uri2->getFragment());

?>
--EXPECT--
NULL
string(6) "%3C%3E"
