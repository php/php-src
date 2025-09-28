--TEST--
Test Uri\WhatWg\Url component modification - fragment - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\WhatWg\Url::parse("https://example.com");
$uri2 = $uri1->withFragment("foo%3dbar"); // foo=bar

var_dump($uri1->getFragment());
var_dump($uri2->getFragment());

?>
--EXPECT--
NULL
string(9) "foo%3dbar"
