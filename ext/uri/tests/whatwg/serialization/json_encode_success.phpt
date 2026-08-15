--TEST--
Test JSON encoding Uri\WhatWg\Url
--FILE--
<?php

var_dump(json_encode(new Uri\WhatWg\Url("https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists")));

?>
--EXPECT--
string(2) "{}"
