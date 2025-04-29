--TEST--
GH-18458 authorization header is set despite CURLOPT_USERPWD set to null
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
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$response = curl_exec($ch);
var_dump(str_contains($response, "authorization"));
?>
--EXPECT--
bool(false)
