--TEST--
Basic curl persistent share handle test
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

$sh1 = curl_share_init_persistent([CURL_LOCK_DATA_CONNECT, CURL_LOCK_DATA_DNS, CURL_LOCK_DATA_DNS]);
$ch1 = get_localhost_curl_handle($sh1);

// Expect the two options we set above, but sorted and de-duplicated.
var_dump($sh1->options);

$sh2 = curl_share_init_persistent([CURL_LOCK_DATA_DNS, CURL_LOCK_DATA_CONNECT]);
$ch2 = get_localhost_curl_handle($sh2);

// Expect the connect time on the subsequent request to be zero, since it's reusing the connection.
var_dump(curl_exec($ch1));
var_dump(curl_exec($ch2));
var_dump("second connect_time: " . (curl_getinfo($ch2)["connect_time"]));

?>
--EXPECT--
array(2) {
  [0]=>
  int(3)
  [1]=>
  int(5)
}
string(23) "Caddy is up and running"
string(23) "Caddy is up and running"
string(22) "second connect_time: 0"
