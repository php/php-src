--TEST--
FR #77711 (CURLFile should support UNICODE filenames)
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$ch = curl_init();
curl_setopt($ch, CURLOPT_SAFE_UPLOAD, 1);
curl_setopt($ch, CURLOPT_URL, "{$host}/get.php?test=file");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

$filename = __DIR__ . '/АБВ.txt';
file_put_contents($filename, "Test.");
$file = curl_file_create($filename);
$params = array('file' => $file);
var_dump(curl_setopt($ch, CURLOPT_POSTFIELDS, $params));

var_dump(curl_exec($ch));
curl_close($ch);
?>
--EXPECTF--
bool(true)
string(%d) "АБВ.txt|application/octet-stream|5"
--CLEAN--
<?php
@unlink(__DIR__ . '/АБВ.txt');
?>
