--TEST--
curl_getinfo CURLINFO_HEADER_OUT
--DESCRIPTION--
Verify that CURLINFO_HEADER_OUT returns the request header sent on the
last request, and that toggling it back to 0 returns FALSE.
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$ch = curl_init("{$host}/get.inc?test=method");
curl_setopt($ch, CURLINFO_HEADER_OUT, 1);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_HTTPHEADER, ['Request-num: 1']);
curl_exec($ch);

// The header string contains the full request header line(s).
$headers = rtrim(curl_getinfo($ch, CURLINFO_HEADER_OUT));
echo "With CURLINFO_HEADER_OUT=1:\n";
echo $headers;
echo "\n---\n";

// Toggling back to 0 should clear the buffer; next getinfo returns false.
curl_setopt($ch, CURLOPT_HTTPHEADER, ['Request-num: 2']);
curl_setopt($ch, CURLINFO_HEADER_OUT, 0);
curl_exec($ch);

$headers = curl_getinfo($ch, CURLINFO_HEADER_OUT);
echo "With CURLINFO_HEADER_OUT=0:\n";
var_dump($headers);

?>
--EXPECTF--
With CURLINFO_HEADER_OUT=1:
GET /get.inc?test=method HTTP/1.1
Host: localhost:%d
%s
Request-num: 1
---
With CURLINFO_HEADER_OUT=0:
bool(false)
