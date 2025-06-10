--TEST--
Test JSON encoding
--EXTENSIONS--
uri
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
var_dump(json_encode($url));

?>
--EXPECT--
string(2) "{}"
