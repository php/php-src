--TEST--
Bug #79033 (Curl timeout error with specific url and post)
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init();
curl_setopt_array($ch, [
    CURLOPT_URL => "{$host}/get.inc?test=post",
    CURLOPT_POST => true,
    CURLOPT_POSTFIELDS => [],
    CURLINFO_HEADER_OUT => true,
    CURLOPT_RETURNTRANSFER => true,
]);
var_dump(curl_exec($ch));
var_dump(curl_getinfo($ch)["request_header"]);
?>
--EXPECTF--
string(%d) "array(0) {
}
"
string(%d) "POST /get.inc?test=post HTTP/1.1
Host: localhost:%d
Accept: */*
Content-Length: 0
Content-Type: application/x-www-form-urlencoded

"
