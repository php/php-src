--TEST--
Test Uri\WhatWg\Url component modification - fragment - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\WhatWg\Url::parse("https://example.com#foo");
$uri2 = $uri1->withFragment(null);

var_dump($uri1->getFragment());
var_dump($uri2->getFragment());

?>
--EXPECT--
string(3) "foo"
NULL
