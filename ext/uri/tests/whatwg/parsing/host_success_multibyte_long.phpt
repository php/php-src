--TEST--
Test Uri\WhatWg\Url parsing - host - IDN host longer than the IDNA on-stack buffer
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://" . str_repeat("é", 5000) . ".com/");
$host = $url->getAsciiHost();

var_dump(strlen($host));
var_dump(substr($host, 0, 12));
var_dump(substr($host, -6));
var_dump(substr_count($host, "a"));
var_dump($url->getUnicodeHost() === str_repeat("é", 5000) . ".com");

?>
--EXPECT--
int(5010)
string(12) "xn--9caaaaaa"
string(6) "aa.com"
int(5000)
bool(true)
