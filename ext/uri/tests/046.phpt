--TEST--
Test special path variants
--EXTENSIONS--
uri
--FILE--
<?php

$url = new Uri\Whatwg\Url("mailto:johndoe@example.com");

var_dump($url->toAsciiString());
var_dump($url->getPath());

$url = new Uri\Whatwg\Url("https://example.com");

var_dump($url->toAsciiString());
var_dump($url->getPath());

$url = new Uri\Whatwg\Url("https://example.com/");

var_dump($url->toAsciiString());
var_dump($url->getPath());

?>
--EXPECT--
string(26) "mailto:johndoe@example.com"
string(19) "johndoe@example.com"
string(20) "https://example.com/"
string(1) "/"
string(20) "https://example.com/"
string(1) "/"
