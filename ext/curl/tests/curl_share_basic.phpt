--TEST--
Basic curl_share test
--EXTENSIONS--
curl
--FILE--
<?php

$sh = curl_share_init();

$ch1 = curl_init();
curl_setopt($ch1, CURLOPT_URL, 'file://' . __DIR__ . '/curl_testdata1.txt');
curl_setopt($ch1, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch1, CURLOPT_SHARE, $sh);

$ch2 = curl_init();
curl_setopt($ch2, CURLOPT_URL, 'file://' . __DIR__ . '/curl_testdata2.txt');
curl_setopt($ch2, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch2, CURLOPT_SHARE, $sh);

// Make sure nothing bad handles if the share handle is unset early.
unset($sh);

var_dump(curl_exec($ch1));
var_dump(curl_exec($ch2));

?>
--EXPECT--
string(6) "CURL1
"
string(6) "CURL2
"
