--TEST--
Expect 100-continue behavior in PHP development server (sockets)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start();

echo "# Send Expect: 100-continue header, receive 100 Continue response.\n";
$fp = php_cli_server_connect();
fwrite($fp, "POST / HTTP/1.1\r\nExpect: 100-continue\r\nContent-Length: 4\r\nConnection: close\r\n\r\n");
echo fgets($fp);
echo fgets($fp);
fwrite($fp, "body");
echo fgets($fp);
fclose($fp);

echo "# Send Expect: 100-continue header and disconnect.\n";
$fp = php_cli_server_connect();
if (extension_loaded('sockets')) {
    // Set SO_LINGER timeout to zero so that send fails on the server immediately
    socket_set_option(
        socket_import_stream($fp),
        SOL_SOCKET,
        SO_LINGER,
        [
            'l_onoff' => 1,
            'l_linger' => 0,
        ]
    );
}
stream_socket_shutdown($fp, STREAM_SHUT_RD);
fwrite($fp, "POST / HTTP/1.1\r\nExpect: 100-continue\r\nContent-Length: 4\r\nConnection: close\r\n\r\n");
fclose($fp);

$fp = php_cli_server_connect();
fwrite($fp, "GET / HTTP/1.1\r\nConnection: close\r\n\r\n");
echo fgets($fp);
fclose($fp);

echo "# GET with Expect header (no body).\n";
$fp = php_cli_server_connect();
fwrite($fp, "GET / HTTP/1.1\r\nExpect: 100-continue\r\nConnection: close\r\n\r\n");
echo fgets($fp);
fclose($fp);

echo "# POST with empty body.\n";
$fp = php_cli_server_connect();
fwrite($fp, "POST / HTTP/1.1\r\nExpect: 100-continue\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
echo fgets($fp);
fclose($fp);

echo "# Lower-case expect header.\n";
$fp = php_cli_server_connect();
fwrite($fp, "POST / HTTP/1.1\r\nexpect: 100-continue\r\nContent-Length: 4\r\nConnection: close\r\n\r\n");
echo fgets($fp);
fclose($fp);
?>
--EXPECT--
# Send Expect: 100-continue header, receive 100 Continue response.
HTTP/1.1 100 Continue

HTTP/1.1 200 OK
# Send Expect: 100-continue header and disconnect.
HTTP/1.1 200 OK
# GET with Expect header (no body).
HTTP/1.1 200 OK
# POST with empty body.
HTTP/1.1 200 OK
# Lower-case expect header.
HTTP/1.1 100 Continue
