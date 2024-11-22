--TEST--
Basic curl_share test
--EXTENSIONS--
curl
--SKIPIF--
<?php
include 'skipif-nocaddy.inc';
?>
--FILE--
<?php

function get_persistent_share_handle(): CurlShareHandle {
    return curl_share_init(
        [
            CURL_LOCK_DATA_CONNECT,
        ],
        "persistent-test",
    );
}

$sh1 = get_persistent_share_handle();
$ch1 = curl_init("https://localhost");

curl_setopt($ch1, CURLOPT_SSL_VERIFYPEER, false);
curl_setopt($ch1, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch1, CURLOPT_SHARE, $sh1);

$sh2 = get_persistent_share_handle();
$ch2 = curl_init("https://localhost");

curl_setopt($ch2, CURLOPT_SSL_VERIFYPEER, false);
curl_setopt($ch2, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch2, CURLOPT_SHARE, $sh2);

var_dump(curl_exec($ch1));
var_dump(curl_exec($ch2));

var_dump("second connect_time: " . (curl_getinfo($ch2)["connect_time"]));

?>
--EXPECT--
string(23) "Caddy is up and running"
string(23) "Caddy is up and running"
string(22) "second connect_time: 0"
