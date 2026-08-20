--TEST--
Test casting Uri\WhatWg\Url to array
--FILE--
<?php

var_dump((array) new Uri\WhatWg\Url("https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists"));

?>
--EXPECTF--
array(%d) {
}
