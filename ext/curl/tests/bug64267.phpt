--TEST--
Bug #64267 (CURLOPT_INFILE doesn't allow reset)
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) die("skip online test");
?>
--FILE--
<?php

echo "TEST\n";

$c = curl_init("http://google.com");
$f = fopen(__FILE__,"r");
var_dump(curl_setopt_array($c, [
    CURLOPT_RETURNTRANSFER => true,
    CURLOPT_UPLOAD => true,
    CURLOPT_INFILE => $f,
    CURLOPT_INFILESIZE => filesize(__FILE__),
    CURLOPT_CONNECTTIMEOUT => 3,
    CURLOPT_TIMEOUT => 3,
]));
fclose($f);
var_dump(curl_setopt_array($c, [
    CURLOPT_UPLOAD => false,
    CURLOPT_INFILE => null,
    CURLOPT_INFILESIZE => 0,
]));
curl_exec($c);
var_dump(curl_getinfo($c, CURLINFO_RESPONSE_CODE));
?>
--EXPECTF--
TEST
bool(true)
bool(true)
int(30%d)
