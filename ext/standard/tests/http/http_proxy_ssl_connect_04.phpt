--TEST--
GH-8832 (HTTP proxy SSL CONNECT uses the http.protocol_version context option)
--EXTENSIONS--
openssl
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$responses = array(
    "data://text/plain,HTTP/1.1 200 Connection established\r\n\r\n",
    "data://text/plain,",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

$host = parse_url($uri, PHP_URL_HOST);
$port = parse_url($uri, PHP_URL_PORT);

$ctx = stream_context_create([
    'http' => [
        'proxy' => "tcp://$host:$port",
        'protocol_version' => 1.1,
        'header' => "Proxy-Authorization: Basic dXNlcjpwYXNz",
    ],
    'ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
    ],
]);
@file_get_contents("https://www.php.net/test", false, $ctx);

http_server_kill($pid);

rewind($output);
$request = stream_get_contents($output);
var_dump(strstr($request, "\r\n\r\n", true));
?>
--EXPECT--
string(96) "CONNECT www.php.net:443 HTTP/1.1
Host: www.php.net:443
Proxy-Authorization: Basic dXNlcjpwYXNz"
