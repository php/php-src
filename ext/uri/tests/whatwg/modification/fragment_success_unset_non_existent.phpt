--TEST--
Test Uri\WhatWg\Url component modification - fragment - unsetting not-existent
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\WhatWg\Url::parse("https://example.com");
$uri2 = $uri1->withFragment(null);

var_dump($uri1->getFragment());
var_dump($uri2->getFragment());

?>
--EXPECT--
NULL
NULL
