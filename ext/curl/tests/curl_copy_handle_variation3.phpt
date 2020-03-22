--TEST--
curl_copy_handle() allows to post CURLFile multiple times
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

$filename = __DIR__ . '/АБВ.txt';
file_put_contents($filename, "Test.");
$file = curl_file_create($filename);
$params = array('file' => $file);
var_dump(curl_setopt($ch1, CURLOPT_POSTFIELDS, $params));

$ch2 = curl_copy_handle($ch1);

var_dump(curl_exec($ch1));
curl_close($ch1);

var_dump(curl_exec($ch2));
curl_close($ch2);
?>
--EXPECTF--
bool(true)
string(%d) "АБВ.txt|application/octet-stream|5"
string(%d) "АБВ.txt|application/octet-stream|5"
--CLEAN--
<?php
@unlink(__DIR__ . '/АБВ.txt');
?>
