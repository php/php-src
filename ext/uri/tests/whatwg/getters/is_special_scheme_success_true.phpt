--TEST--
Test Uri\WhatWg\Url::isSpecialScheme() - success - special
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("http://example.com");
var_dump($url->isSpecialScheme());

$url = Uri\WhatWg\Url::parse("https://example.com");
var_dump($url->isSpecialScheme());

$url = Uri\WhatWg\Url::parse("ws://example.com");
var_dump($url->isSpecialScheme());

$url = Uri\WhatWg\Url::parse("wss://example.com");
var_dump($url->isSpecialScheme());

$url = Uri\WhatWg\Url::parse("ftp://example.com");
var_dump($url->isSpecialScheme());

$url = Uri\WhatWg\Url::parse("file://example.com");
var_dump($url->isSpecialScheme());

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
