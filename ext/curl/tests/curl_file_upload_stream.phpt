--TEST--
CURL file uploading from stream
--SKIPIF--
<?php include 'skipif.inc'; ?>
<?php
if (curl_version()['version_number'] < 0x73800) die('skip requires curl >= 7.56.0');
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$ch = curl_init();
curl_setopt($ch, CURLOPT_SAFE_UPLOAD, 1);
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=file");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

$file = curl_file_create('data://text/plain;base64,SSBsb3ZlIFBIUAo=', 'text/plain', 'i-love-php');
$params = array('file' => $file);
var_dump(curl_setopt($ch, CURLOPT_POSTFIELDS, $params));

var_dump(curl_exec($ch));
curl_close($ch);
?>
--EXPECT--
bool(true)
string(24) "i-love-php|text/plain|11"
