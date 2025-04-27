--TEST--
CURLOPT_FOLLOWLOCATION values
--EXTENSIONS--
curl
--SKIPIF--
<?php
include 'skipif-nocaddy.inc';
if (curl_version()['version_number'] < 0x080d00) die('skip requires curl >= 8.13.0');
?>
--FILE--
<?php

for ($i = 0; $i <= CURLFOLLOW_FIRSTONLY; $i ++) {
	$ch = curl_init();
	curl_setopt($ch,CURLOPT_URL, "https://localhost/redirect");
	curl_setopt($ch,CURLOPT_RETURNTRANSFER,true);
	var_dump(curl_setopt($ch, CURLOPT_FOLLOWLOCATION, $i));
	var_dump(curl_exec($ch));
	curl_close($ch);
}
?>
--EXPECTF--
bool(true)
string(%d) "%s"
bool(true)
string(%d) "%s"
bool(true)
string(%d) "%s"
bool(true)
string(%d) "%s"

