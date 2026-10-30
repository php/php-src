--TEST--
GH-23814 / GH-23860 (A curl callback can replace itself)
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (!in_array('file', curl_version()['protocols'], true)) {
    die('skip file protocol not supported');
}
?>
--FILE--
<?php

$handle = curl_init('file://' . __FILE__);
$callback = static function (CurlHandle $handle, string $data): int {
    echo "Original callback\n";
    var_dump(curl_setopt($handle, CURLOPT_WRITEFUNCTION, null));
    var_dump(curl_setopt_array($handle, [CURLOPT_WRITEFUNCTION =>
        static function (CurlHandle $handle, string $data): int {
            echo "Replacement callback\n";
            return strlen($data);
        },
    ]));

    return strlen($data);
};
curl_setopt($handle, CURLOPT_WRITEFUNCTION, $callback);
var_dump(curl_exec($handle));
var_dump(curl_exec($handle));
var_dump(curl_setopt($handle, CURLOPT_WRITEFUNCTION, null));
?>
--EXPECT--
Original callback
bool(true)
bool(true)
bool(true)
Replacement callback
bool(true)
bool(true)
