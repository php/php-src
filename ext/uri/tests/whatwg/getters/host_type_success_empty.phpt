--TEST--
Test Uri\WhatWg\Url component retrieval - host type - empty
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("file:///foo/bar");

var_dump($url->getHostType());

?>
--EXPECT--
enum(Uri\WhatWg\UrlHostType::Empty)
