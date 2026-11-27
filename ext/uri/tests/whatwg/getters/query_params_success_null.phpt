--TEST--
Test Uri\WhatWg\Url::getQueryParams() - success - an absent query returns null
--FILE--
<?php

$uri = new Uri\WhatWg\Url("https://example.com");

var_dump($uri->getQueryParams());

?>
--EXPECT--
NULL
