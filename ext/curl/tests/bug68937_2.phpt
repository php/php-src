--TEST--
Bug # #68937 (Segfault in curl_multi_exec)
--SKIPIF--
<?php
include 'skipif.inc';
?>
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$url = "{$host}/get.inc";

$ch = curl_init($url);
curl_setopt_array($ch, array(
	CURLOPT_HEADER => false,
	CURLOPT_RETURNTRANSFER => true,
	CURLOPT_POST => true,
	CURLOPT_INFILESIZE => filesize(__FILE__),
	CURLOPT_INFILE => fopen(__FILE__, 'r'),
	CURLOPT_HTTPHEADER => array(
		'Expect:',
		'Content-Length: 1',
	),
	CURLOPT_READFUNCTION => 'curl_read',
	CURLOPT_CONNECTTIMEOUT => 1,
	CURLOPT_TIMEOUT => 1
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
