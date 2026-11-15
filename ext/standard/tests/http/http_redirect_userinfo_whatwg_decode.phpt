--TEST--
Redirect userinfo is decoded into a new string with the WHATWG URL parser
--FILE--
<?php
$serverCode = <<<'CODE'
$server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr);
phpt_notify_server_start($server);
$addr = stream_socket_get_name($server, false);

/* a one-character user, then a one-character password */
$locations = ["http://+@$addr/b", "http://a:+@$addr/d"];

for ($n = 0; $n < 4; $n++) {
    $conn = stream_socket_accept($server, 10);
    if (!$conn) {
        break;
    }
    $auth = '';
    fgets($conn);
    while (($line = trim(fgets($conn))) !== '') {
        if (stripos($line, 'Authorization:') === 0) {
            $auth = $line;
        }
    }
    if ($n % 2 == 0) {
        fwrite($conn, "HTTP/1.1 302 Found\r\nLocation: {$locations[$n / 2]}\r\nContent-Length: 0\r\n\r\n");
    } else {
        fwrite($conn, "HTTP/1.1 200 OK\r\nContent-Length: " . strlen($auth) . "\r\n\r\n$auth");
    }
    fclose($conn);
}
CODE;

$clientCode = <<<'CODE'
$ctx = stream_context_create(['http' => ['follow_location' => 1, 'uri_parser_class' => \Uri\WhatWg\Url::class]]);
echo file_get_contents("http://{{ ADDR }}/a", false, $ctx), "\n";
var_dump(bin2hex(chr(43)));
echo file_get_contents("http://{{ ADDR }}/c", false, $ctx), "\n";
var_dump(bin2hex(chr(43)));
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
Authorization: Basic IDo=
string(2) "2b"
Authorization: Basic YTog
string(2) "2b"
