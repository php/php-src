--TEST--
GH-23814 (Setting a callback option from within a curl callback is rejected)
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
    try {
        curl_setopt($handle, CURLOPT_WRITEFUNCTION, static fn($handle, $data) => strlen($data));
    } catch (Error $error) {
        echo $error->getMessage(), "\n";
    }

    try {
        curl_setopt_array($handle, [CURLOPT_WRITEFUNCTION => null]);
    } catch (Error $error) {
        echo $error->getMessage(), "\n";
    }

    return strlen($data);
};
curl_setopt($handle, CURLOPT_WRITEFUNCTION, $callback);
var_dump(curl_exec($handle));
var_dump(curl_setopt($handle, CURLOPT_WRITEFUNCTION, null));
?>
--EXPECT--
curl_setopt(): Attempt to set the CURLOPT_WRITEFUNCTION option from a callback
curl_setopt_array(): Attempt to set the CURLOPT_WRITEFUNCTION option from a callback
bool(true)
bool(true)
