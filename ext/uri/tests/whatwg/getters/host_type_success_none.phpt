--TEST--
Test Uri\WhatWg\Url component retrieval - host type - none
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("email:john.doe@example.com");

var_dump($url->getHostType());

?>
--EXPECT--
NULL
