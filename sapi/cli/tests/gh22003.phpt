--TEST--
GH-22003 (CLI server: overflow in Content-Length parser + post_max_size enforcement)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start("echo 'OK';", null, ['-d', 'post_max_size=1024']);

$host = PHP_CLI_SERVER_HOSTNAME;

// 1. Content-Length above the configured post_max_size but within ssize_t:
//    consumer must reject with a 413 page before allocating a body buffer.
$fp = php_cli_server_connect();
fwrite($fp, "POST / HTTP/1.1\r\nHost: $host\r\nContent-Length: 999999\r\nConnection: close\r\n\r\n");
$response = stream_get_contents($fp);
fclose($fp);
echo "over post_max_size: ", str_contains($response, "413 Request Entity Too Large") ? "413" : "FAIL", "\n";
echo "shows configured limit: ", str_contains($response, "1024 bytes") ? "yes" : "no", "\n";

// 2. Same case but with body bytes piggybacked in the same write. The parser sees
//    the body bytes after on_headers_complete bails; without a guard in the
//    read-request error path the response would be 400 instead of 413.
$fp = php_cli_server_connect();
fwrite($fp, "POST / HTTP/1.1\r\nHost: $host\r\nContent-Length: 999999\r\nConnection: close\r\n\r\n0123456789");
$response = stream_get_contents($fp);
fclose($fp);
echo "over limit with body bytes: ", str_contains($response, "413 Request Entity Too Large") ? "413" : "FAIL", "\n";

// 3. Content-Length wide enough to overflow ssize_t accumulation in the parser:
//    parser-level guard rejects as a malformed request before headers-complete fires.
$fp = php_cli_server_connect();
fwrite($fp, "POST / HTTP/1.1\r\nHost: $host\r\nContent-Length: 999999999999999999999999999999\r\nConnection: close\r\n\r\n");
$response = stream_get_contents($fp);
fclose($fp);
echo "content-length overflow: ", str_contains($response, "200 OK") ? "FAIL" : "rejected", "\n";

// 4. Transfer-Encoding: chunked with an oversized hex chunk size: same parser guard
//    on the chunked accumulator must reject without aborting the server.
$fp = php_cli_server_connect();
fwrite($fp, "POST / HTTP/1.1\r\nHost: $host\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"
    . str_repeat("F", 32) . "\r\n");
$response = stream_get_contents($fp);
fclose($fp);
echo "chunked overflow: ", str_contains($response, "200 OK") ? "FAIL" : "rejected", "\n";

// 5. Chunked body whose accumulated size exceeds post_max_size. No Content-Length
//    header exists, so the limit can only be enforced as the chunks arrive.
$fp = php_cli_server_connect();
$request = "POST / HTTP/1.1\r\nHost: $host\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n";
for ($i = 0; $i < 4; $i++) {
    $request .= sprintf("%x\r\n%s\r\n", 1000, str_repeat("A", 1000));
}
fwrite($fp, $request . "0\r\n\r\n");
$response = stream_get_contents($fp);
fclose($fp);
echo "chunked over post_max_size: ", str_contains($response, "413 Request Entity Too Large") ? "413" : "FAIL", "\n";

// 6. Chunk size under the ssize_t guard but far above post_max_size, with only a few
//    body bytes behind it: the buffer must grow with the bytes that arrive, never
//    from the declared chunk size.
$fp = php_cli_server_connect();
fwrite($fp, "POST / HTTP/1.1\r\nHost: $host\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"
    . "7FFFFFFFFFFFFF\r\nHELLO");
stream_socket_shutdown($fp, STREAM_SHUT_WR);
$response = stream_get_contents($fp);
fclose($fp);
echo "oversize chunk size: ", str_contains($response, "200 OK") ? "FAIL" : "rejected", "\n";

// 7. A chunked body within the limit still reaches the script.
$fp = php_cli_server_connect();
fwrite($fp, "POST / HTTP/1.1\r\nHost: $host\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"
    . sprintf("%x\r\n%s\r\n", 100, str_repeat("B", 100)) . "0\r\n\r\n");
$response = stream_get_contents($fp);
fclose($fp);
echo "chunked within limit: ", str_contains($response, "200 OK") ? "200 OK" : "FAIL", "\n";

// 8. Server must still be alive and serving normal requests.
$fp = php_cli_server_connect();
fwrite($fp, "GET / HTTP/1.1\r\nHost: $host\r\nConnection: close\r\n\r\n");
$response = stream_get_contents($fp);
fclose($fp);
echo "follow-up: ", str_contains($response, "200 OK") ? "200 OK" : "FAILED", "\n";
?>
--EXPECT--
over post_max_size: 413
shows configured limit: yes
over limit with body bytes: 413
content-length overflow: rejected
chunked overflow: rejected
chunked over post_max_size: 413
oversize chunk size: rejected
chunked within limit: 200 OK
follow-up: 200 OK
