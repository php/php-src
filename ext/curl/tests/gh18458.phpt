--TEST--
GH-18458 (authorization header is set despite CURLOPT_USERPWD set to null)
--EXTENSIONS--
curl
--SKIPIF--
<?php
include 'skipif-nocaddy.inc';
?>
--FILE--
<?php

$ch = curl_init("https://localhost/userpwd");
curl_setopt($ch, CURLOPT_USERPWD, null);
curl_setopt($ch, CURLOPT_VERBOSE, true);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_STDERR, fopen("php://stdout", "w"));
$response = curl_exec($ch);
var_dump(str_contains($response, "authorization"));

$ch = curl_init("https://localhost/username");
curl_setopt($ch, CURLOPT_USERNAME, null);
curl_setopt($ch, CURLOPT_PASSWORD, null);
curl_setopt($ch, CURLOPT_VERBOSE, true);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_STDERR, fopen("php://stdout", "w"));
$response = curl_exec($ch);
var_dump(str_contains($response, "authorization"));
?>
--EXPECTF--
%A
bool(false)
%A
bool(false)
