--TEST--
Curlinfo CURLINFO_CONN_ID
--EXTENSIONS--
curl
--SKIPIF--
<?php
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x080200) die("skip: test works only with curl >= 8.2.0");
?>
--FILE--
<?php
include 'server.inc';

$host = curl_cli_server_start();
$port = (int) (explode(':', $host))[1];

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=file");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_FORBID_REUSE, false);
curl_setopt($ch, CURLOPT_FRESH_CONNECT, false);
curl_setopt($ch, CURLOPT_HTTPHEADER, [
    'Connection: Keep-Alive',
    'Keep-Alive: 10'
]);

$info = curl_getinfo($ch);
var_dump(isset($info['conn_id']));
var_dump($info['conn_id'] === 0);
var_dump("1. debug");
var_dump($info['conn_id']);
print_r($info);

$result = curl_exec($ch);

$info = curl_getinfo($ch);
var_dump(isset($info['conn_id']));
var_dump(is_int($info['conn_id']));
var_dump(curl_getinfo($ch, CURLINFO_CONN_ID) === $info['conn_id']);
var_dump(curl_getinfo($ch, CURLINFO_CONN_ID) === -1);
var_dump("2. debug");
var_dump($info['conn_id']);
print_r($info);



curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=file");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$result = curl_exec($ch);

$info = curl_getinfo($ch);
var_dump("3. debug");
var_dump($info['conn_id']);
print_r($info);


?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

