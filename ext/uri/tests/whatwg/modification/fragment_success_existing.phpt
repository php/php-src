--TEST--
Test Uri\WhatWg\Url component modification - fragment - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\WhatWg\Url::parse("https://example.com#foo");
$uri2 = $uri1->withFragment("bar");

var_dump($uri1->getFragment());
var_dump($uri2->getFragment());

?>
--EXPECT--
string(3) "foo"
string(3) "bar"
