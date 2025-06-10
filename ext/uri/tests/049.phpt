--TEST--
Test percent-encoding normalization - special case
--EXTENSIONS--
uri
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://example.com/foo/bar%2Fbaz");
var_dump($url->getPath());

?>
--EXPECT--
string(14) "/foo/bar%2Fbaz"
