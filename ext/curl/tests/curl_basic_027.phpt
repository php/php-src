--TEST--
Test curl_getinfo() function with CURLINFO_* and CURLOPT_* from curl >= 7.84.0
--EXTENSIONS--
curl
--SKIPIF--
<?php $curl_version = curl_version();
if ($curl_version['version_number'] < 0x075400) {
    exit("skip: test works only with curl >= 7.84.0");
}
?>
--FILE--
<?php
include 'server.inc';

$ch = curl_init();
var_dump(array_key_exists('capath', curl_getinfo($ch)));
var_dump(array_key_exists('cainfo', curl_getinfo($ch)));

$host = curl_cli_server_start();

$url = "{$host}/get.inc?test=";
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
var_dump(curl_setopt($ch, CURLOPT_SSH_HOSTKEYFUNCTION, function ($ch, $keytype, $key, $keylen) {
    // Can't really trigger this in a test
    var_dump($keytype);
    var_dump($key);
    var_dump($keylen);
    return CURLKHMATCH_OK;
}));
curl_exec($ch);
curl_close($ch);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
