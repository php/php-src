--TEST--
Test array cast
--EXTENSIONS--
uri
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
var_dump((array) $uri);

$url = new Uri\WhatWg\Url("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
var_dump((array) $url);

?>
--EXPECTF--
array(%d) {
}
array(%d) {
}
