--TEST--
curl seek within uploaded file
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$url = "{$host}/get.inc?test=redirect&target=file";

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
curl_setopt($ch, CURLOPT_POSTFIELDS, [
    'file' => new CURLFile(__DIR__ . '/curl_testdata1.txt')
]);
curl_exec($ch);

?>
--EXPECT--
curl_testdata1.txt|application/octet-stream|6
