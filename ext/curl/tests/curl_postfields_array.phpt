--TEST--
CURLOPT_POSTFIELDS with multi-value fields
--EXTENSIONS--
curl
sockets
--FILE--
<?php
$socket = stream_socket_server("tcp://0.0.0.0:29999", $errno, $errstr);

if (!$socket) {
	echo "$errstr ($errno)<br />\n";
	return;
}

$url = "http://127.0.0.1:29999/get.inc?test=raw";

$fields = [
	'single' => 'SingleValue',
	'multi' => [
		'Multi1',
		'Multi2',
	]
];

$options = [
	CURLOPT_POST => 1,
	CURLOPT_HEADER => 0,
	CURLOPT_URL => $url,
	CURLOPT_FRESH_CONNECT => 1,
	CURLOPT_RETURNTRANSFER => 1,
	CURLOPT_FORBID_REUSE => 1,
	CURLOPT_TIMEOUT => 1,
	CURLOPT_POSTFIELDS => $fields,
	CURLOPT_HTTPHEADER => [ 'Expect:' ],
];

$ch = curl_init();
curl_setopt_array($ch, $options);

$curl_content = curl_exec($ch);
curl_close($ch);
	
$conn = stream_socket_accept($socket);
echo stream_get_contents($conn);
?>
--EXPECTF--
POST /get.inc?test=raw HTTP/1.1
Host: %s
Accept: */*
Content-Length: %d
Content-Type: multipart/form-data; boundary=------------------------%s

--------------------------%s
Content-Disposition: form-data; name="single"

SingleValue
--------------------------%s
Content-Disposition: form-data; name="multi"

Multi1
--------------------------%s
Content-Disposition: form-data; name="multi"

Multi2
--------------------------%s--
