--TEST--
Test Uri\WhatWg\Url parsing - username - at sign in the username and the password
--FILE--
<?php

$url = new Uri\WhatWg\Url("http://user@name:pass@word@localhost/");

var_dump($url->getUsername());
var_dump($url->getPassword());
var_dump($url->getAsciiHost());
var_dump($url->toAsciiString());

?>
--EXPECT--
string(11) "user%40name"
string(11) "pass%40word"
string(9) "localhost"
string(41) "http://user%40name:pass%40word@localhost/"
