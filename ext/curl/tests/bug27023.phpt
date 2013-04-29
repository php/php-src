--TEST--
Bug #27023 (CURLOPT_POSTFIELDS does not parse content types for files)
--INI--
error_reporting = E_ALL & ~E_DEPRECATED
--SKIPIF--
<?php 
if (!extension_loaded("curl")) {
	exit("skip curl extension not loaded");
}
if (false === getenv('PHP_CURL_HTTP_REMOTE_SERVER'))  {
	exit("skip PHP_CURL_HTTP_REMOTE_SERVER env variable is not defined");
}
?>
--FILE--
<?php

$host = getenv('PHP_CURL_HTTP_REMOTE_SERVER');
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.php?test=file");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

$params = array('file' => '@' . __DIR__ . '/curl_testdata1.txt');
curl_setopt($ch, CURLOPT_POSTFIELDS, $params);
var_dump(curl_exec($ch));

$params = array('file' => '@' . __DIR__ . '/curl_testdata1.txt;type=text/plain');
curl_setopt($ch, CURLOPT_POSTFIELDS, $params);
var_dump(curl_exec($ch));

$params = array('file' => '@' . __DIR__ . '/curl_testdata1.txt;filename=foo.txt');
curl_setopt($ch, CURLOPT_POSTFIELDS, $params);
var_dump(curl_exec($ch));

$params = array('file' => '@' . __DIR__ . '/curl_testdata1.txt;type=text/plain;filename=foo.txt');
curl_setopt($ch, CURLOPT_POSTFIELDS, $params);
var_dump(curl_exec($ch));

$params = array('file' => '@' . __DIR__ . '/curl_testdata1.txt;filename=foo.txt;type=text/plain');
curl_setopt($ch, CURLOPT_POSTFIELDS, $params);
var_dump(curl_exec($ch));


curl_close($ch);
?>
--EXPECTF--
string(%d) "curl_testdata1.txt|application/octet-stream"
string(%d) "curl_testdata1.txt|text/plain"
string(%d) "foo.txt|application/octet-stream"
string(%d) "foo.txt|text/plain"
string(%d) "foo.txt|text/plain"
