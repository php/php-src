--TEST--
Test Uri\WhatWg\Url::isSpecialScheme() - success - not special
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("scheme://example.com");

var_dump($url->isSpecialScheme());

?>
--EXPECT--
bool(false)
