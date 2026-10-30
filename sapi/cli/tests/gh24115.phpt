--TEST--
GH-24115 (CLI server: chunked request trailers are not limited by the header size limit)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start("echo 'OK';");

$host = PHP_CLI_SERVER_HOSTNAME;
$head = "POST / HTTP/1.1\r\nHost: $host\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n4\r\ndata\r\n";

function send(string $request): string {
    $fp = php_cli_server_connect();
    fwrite($fp, $request);
    $response = stream_get_contents($fp);
    fclose($fp);
    return $response;
}

// A small trailer is still accepted.
$response = send($head . "0\r\nX-Trailer: small\r\n\r\n");
echo "small trailer: ", str_contains($response, "200 OK") ? "200" : "FAIL", "\n";

// A trailer larger than the header size limit is rejected.
$response = send($head . "0\r\nX-Fill: " . str_repeat("A", 100 * 1024) . "\r\n\r\n");
echo "oversized trailer: ", str_contains($response, "200 OK") ? "FAIL" : "rejected", "\n";

// The server keeps serving requests afterwards.
$response = send($head . "0\r\n\r\n");
echo "after rejection: ", str_contains($response, "200 OK") ? "200" : "FAIL", "\n";
?>
--EXPECT--
small trailer: 200
oversized trailer: rejected
after rejection: 200
