--TEST--
Test Uri\WhatWg\Url component retrieval - host type - registered name
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com");

var_dump($url->getHostType());

?>
--EXPECT--
enum(Uri\WhatWg\UrlHostType::Domain)
