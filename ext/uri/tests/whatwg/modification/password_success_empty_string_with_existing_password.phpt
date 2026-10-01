--TEST--
Test Uri\WhatWg\Url::withPassword() - success - empty string removes existing password
--FILE--
<?php

$url1 = new Uri\WhatWg\Url('https://user:pass@example.com/path');
$url2 = $url1->withPassword('');

var_dump($url1->getPassword());
var_dump($url2->getPassword());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(4) "pass"
string(0) ""
string(29) "https://user@example.com/path"
