--TEST--
Test Uri\WhatWg\Url component modification - fragment - with leading hashmark
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\WhatWg\Url::parse("https://example.com");
$uri2 = $uri1->withFragment("#fragment");

var_dump($uri1->getFragment());
var_dump($uri2->getFragment());

?>
--EXPECT--
NULL
string(8) "fragment"
