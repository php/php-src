--TEST--
Curl option CURLOPT_WRITEHEADER
--DESCRIPTION--
Test writing HTTP response headers to a file using CURLOPT_WRITEHEADER.
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$header_file = tempnam(sys_get_temp_dir(), 'curl-writeheader');
$fp = fopen($header_file, 'w') or die('failed to open header output file');

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=contenttype");
curl_setopt($ch, CURLOPT_WRITEHEADER, $fp);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_exec($ch);

fclose($fp);

// Verify headers were written to the file
$header_contents = file_get_contents($header_file);
echo $header_contents;
echo "\n";

unlink($header_file);
?>
--EXPECTF--
HTTP/1.1 200 OK
Host: localhost:%d
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-Type: text/plain;charset=utf-8
