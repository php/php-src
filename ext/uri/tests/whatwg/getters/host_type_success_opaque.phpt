--TEST--
Test Uri\WhatWg\Url component retrieval - host type - opaque
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("scheme://example.com");

var_dump($url->getHostType());

?>
--EXPECT--
enum(Uri\WhatWg\UrlHostType::Opaque)
