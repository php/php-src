--TEST--
Curl persistent share handle test with different options
--EXTENSIONS--
curl
--SKIPIF--
<?php
include 'skipif-nocaddy.inc';
?>
--FILE--
<?php

function get_localhost_curl_handle(CurlSharePersistentHandle $sh): CurlHandle {
    $ch = curl_init("https://localhost");

    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_SHARE, $sh);

    return $ch;
}

$sh1 = curl_share_init_persistent([CURL_LOCK_DATA_DNS, CURL_LOCK_DATA_CONNECT]);
$ch1 = get_localhost_curl_handle($sh1);

// Note that we're using different share options and thus should get a different persistent share
// handle.
$sh2 = curl_share_init_persistent([CURL_LOCK_DATA_CONNECT]);
$ch2 = get_localhost_curl_handle($sh2);

var_dump($sh1->options != $sh2->options);

// Expect the connect time on the subsequent request to be non-zero, since it's *not* reusing the connection.
var_dump(curl_exec($ch1));
var_dump(curl_exec($ch2));
var_dump("second connect_time: " . (curl_getinfo($ch2)["connect_time"]));

?>
--EXPECTREGEX--
bool\(true\)
string\(23\) "Caddy is up and running"
string\(23\) "Caddy is up and running"
string\(\d+\) "second connect_time: .*[1-9].*"
