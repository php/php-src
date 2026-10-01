--TEST--
HTTP proxy SSL CONNECT with Proxy-Authorization header (array)
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
    "data://text/plain,HTTP/1.0 200 Connection established\r\n\r\n",
    "data://text/plain,",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

$host = parse_url($uri, PHP_URL_HOST);
$port = parse_url($uri, PHP_URL_PORT);

$ctx = stream_context_create([
    'http' => [
        'proxy' => "tcp://$host:$port",
        'header' => ["X-Custom: test", "Proxy-Authorization: Basic abc123"],
    ],
    'ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
    ],
]);
@$result = file_get_contents("https://www.php.net/test", false, $ctx);
var_dump($result);

http_server_kill($pid);

rewind($output);
$request = stream_get_contents($output);
var_dump(str_contains($request, 'CONNECT www.php.net:443 HTTP/1.0'));
var_dump(str_contains($request, 'Proxy-Authorization: Basic abc123'));
var_dump(str_contains($request, 'X-Custom'));
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(false)
