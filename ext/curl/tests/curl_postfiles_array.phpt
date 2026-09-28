--TEST--
CURLOPT_POSTFIELDS with multi-value files
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
	'multi_string' => [
		new CURLStringFile('Multi1', 'multi1.txt', 'text/plain'),
		new CURLStringFile('Multi2', 'multi2.dat'),
	],
	'multi_file' => [
		new CURLFile(__DIR__ . '/curl_testdata1.txt'),
		new CURLFile(__DIR__ . '/curl_testdata2.txt'),
	],
	'mixed' => [
		'string value',
		new CURLStringFile('curl string file', 'stringfile.txt', 'text/plain'),
		new CURLFile(__DIR__ . '/curl_testdata1.txt', 'text/plain'),
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
Content-Disposition: form-data; name="multi_string"; filename="multi1.txt"
Content-Type: text/plain

Multi1
--------------------------%s
Content-Disposition: form-data; name="multi_string"; filename="multi2.dat"
Content-Type: application/octet-stream

Multi2
--------------------------%s
Content-Disposition: form-data; name="multi_file"; filename="%s"
Content-Type: application/octet-stream

CURL1

--------------------------%s
Content-Disposition: form-data; name="multi_file"; filename="%s"
Content-Type: application/octet-stream

CURL2

--------------------------%s
Content-Disposition: form-data; name="mixed"

string value
--------------------------%s
Content-Disposition: form-data; name="mixed"; filename="stringfile.txt"
Content-Type: text/plain

curl string file
--------------------------%s
Content-Disposition: form-data; name="mixed"; filename="%s"
Content-Type: text/plain

CURL1

--------------------------%s--
