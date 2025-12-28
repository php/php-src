--TEST--
curl_getinfo - CURLINFO_HTTPAUTH_USED
--EXTENSIONS--
curl
--SKIPIF--
<?php
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x080c00) die("skip: test works only with curl >= 8.12.0");
?>
--FILE--
<?php
include 'server.inc';

$host = curl_cli_server_start();
$port = (int) (explode(':', $host))[1];

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=file");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

echo "httpauth_used and proxyauth_used empty\n";

$info = curl_getinfo($ch);
var_dump(isset($info['httpauth_used']));
var_dump(isset($info['proxyauth_used']));
var_dump($info['httpauth_used'] === 0); // this is always 0 before executing the transfer
var_dump($info['proxyauth_used'] === 0); // this is always 0 before executing the transfer

$result = curl_exec($ch);
echo "httpauth_used and proxyauth_used empty after request\n";
$info = curl_getinfo($ch);
var_dump(isset($info['httpauth_used']));
var_dump(isset($info['proxyauth_used']));
var_dump($info['httpauth_used'] === 0);
var_dump($info['proxyauth_used'] === 0);
var_dump(curl_getinfo($ch, CURLINFO_HTTPAUTH_USED) === $info['used_proxy']);
var_dump(curl_getinfo($ch, CURLINFO_PROXYAUTH_USED) === $info['proxyauth_used']);

echo "httpauth_used set after request\n";

$ch = curl_init();
curl_setopt($ch, CURLOPT_HTTPAUTH, CURLAUTH_BASIC | CURLAUTH_DIGEST);
curl_setopt($ch, CURLOPT_USERNAME, "foo");
curl_setopt($ch, CURLOPT_PASSWORD, "bar");

$result = curl_exec($ch);
$info = curl_getinfo($ch);

var_dump($info['httpauth_used']); // Built-in server does not support auth

?>
--EXPECT--
httpauth_used and proxyauth_used empty
bool(true)
bool(true)
bool(true)
bool(true)
httpauth_used and proxyauth_used empty after request
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
httpauth_used set after request
int(0)
