--TEST--
Test Uri\WhatWg\Url component retrieval - host type - IPv4
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://192.168.0.1");

var_dump($url->getHostType());

?>
--EXPECT--
enum(Uri\WhatWg\UrlHostType::IPv4)
