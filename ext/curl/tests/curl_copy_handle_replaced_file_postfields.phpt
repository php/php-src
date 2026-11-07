--TEST--
curl_copy_handle() does not restore file post fields that were replaced
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$handle = curl_init("{$host}/get.inc?test=input");
curl_setopt($handle, CURLOPT_RETURNTRANSFER, true);
curl_setopt($handle, CURLOPT_POSTFIELDS, ['file' => new CURLStringFile('old body', 'old.txt')]);
curl_setopt($handle, CURLOPT_POSTFIELDS, 'new body');

$copy = curl_copy_handle($handle);
echo curl_exec($handle);
echo curl_exec($copy);
?>
--EXPECT--
string(8) "new body"
string(8) "new body"
