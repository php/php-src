--TEST--
Test Uri\WhatWg\Url component retrieval - host type - IPv6
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://[2001:0db8:3333:4444:5555:6666:7777:8888]");

var_dump($url->getHostType());

?>
--EXPECT--
enum(Uri\WhatWg\UrlHostType::IPv6)
