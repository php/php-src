--TEST--
Bug # #68937 (Segfault in curl_multi_exec)
--SKIPIF--
<?php 
if (getenv("SKIP_ONLINE_TESTS")) die("skip online test");
include 'skipif.inc';
?>
--FILE--
<?php

$ch = curl_init('http://www.google.com/');
curl_setopt_array($ch, array(
	CURLOPT_HEADER => false,
	CURLOPT_RETURNTRANSFER => true,
	CURLOPT_POST => true,
	CURLOPT_INFILESIZE => filesize(__FILE__),
	CURLOPT_INFILE => fopen(__FILE__, 'r'),
	CURLOPT_HTTPHEADER => array(
		'Content-Length: 1',
	),
	CURLOPT_READFUNCTION => 'curl_read'
));

function curl_read($ch, $fp, $len) {
	var_dump($fp);
	exit;
}

curl_exec($ch);
curl_close($ch);
?>
--EXPECTF--
resource(%d) of type (stream)
