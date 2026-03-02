--TEST--
GH-20771 (Assertion failure when getUnicodeHost() returns empty string)
--EXTENSIONS--
uri
--FILE--
<?php

$url = Uri\WhatWg\Url::parse('test://');

var_dump($url->getUnicodeHost());

?>
--EXPECT--
string(0) ""
