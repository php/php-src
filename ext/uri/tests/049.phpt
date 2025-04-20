--TEST--
Test percent-encoding normalization - special case
--EXTENSIONS--
uri
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://example.com/foo/bar%2Fbaz");
var_dump($uri->getRawPath());
var_dump($uri->getPath());

$url = new Uri\WhatWg\Url("https://example.com/foo/bar%2Fbaz");
var_dump($url->getRawPath());

?>
--EXPECT--
string(14) "/foo/bar%2Fbaz"
string(14) "/foo/bar%2Fbaz"
string(14) "/foo/bar%2Fbaz"
