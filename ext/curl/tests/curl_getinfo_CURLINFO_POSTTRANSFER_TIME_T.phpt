--TEST--
Curlinfo CURLINFO_POSTTRANSFER_TIME_T
--EXTENSIONS--
curl
--SKIPIF--
<?php
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x080a00) die("skip: test works only with curl >= 8.10.0");
?>
--FILE--
<?php
include 'server.inc';

$host = curl_cli_server_start();
$port = (int) (explode(':', $host))[1];

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=file");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

$info = curl_getinfo($ch);
var_dump(isset($info['posttransfer_time_us']));
var_dump($info['posttransfer_time_us'] === 0); // this is always 0 before executing the transfer

$result = curl_exec($ch);

$info = curl_getinfo($ch);
var_dump(isset($info['posttransfer_time_us']));
var_dump(is_int($info['posttransfer_time_us']));
var_dump(curl_getinfo($ch, CURLINFO_POSTTRANSFER_TIME_T) === $info['posttransfer_time_us']);
var_dump(curl_getinfo($ch, CURLINFO_POSTTRANSFER_TIME_T) > 0);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

