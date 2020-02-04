--TEST--
curl_copy_handle() allows to post CURLFile multiple times if postfields change
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$ch1 = curl_init();
curl_setopt($ch1, CURLOPT_SAFE_UPLOAD, 1);
curl_setopt($ch1, CURLOPT_URL, "{$host}/get.php?test=file");
curl_setopt($ch1, CURLOPT_RETURNTRANSFER, 1);

$filename = __DIR__ . '/abc.txt';
file_put_contents($filename, "Test.");
$file = curl_file_create($filename);
$params = array('file' => $file);
var_dump(curl_setopt($ch1, CURLOPT_POSTFIELDS, $params));

$ch2 = curl_copy_handle($ch1);

$filename = __DIR__ . '/def.txt';
file_put_contents($filename, "Other test.");
$file = curl_file_create($filename);
$params = array('file' => $file);
var_dump(curl_setopt($ch2, CURLOPT_POSTFIELDS, $params));

$ch3 = curl_copy_handle($ch2);

var_dump(curl_exec($ch1));
curl_close($ch1);

var_dump(curl_exec($ch2));
curl_close($ch2);

var_dump(curl_exec($ch3));
curl_close($ch3);
?>
===DONE===
--EXPECTF--
bool(true)
bool(true)
string(%d) "abc.txt|application/octet-stream|5"
string(%d) "def.txt|application/octet-stream|11"
string(%d) "def.txt|application/octet-stream|11"
===DONE===
--CLEAN--
<?php
@unlink(__DIR__ . '/abc.txt');
@unlink(__DIR__ . '/def.txt');
?>
