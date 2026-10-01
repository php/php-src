--TEST--
GH-21031 (Fix NULL deref when enabling TLS fails and the peer name needs to be reset)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die("skip not reliable on Windows due to proxy wait limitation");
}
?>
--FILE--
<?php

$serverCode = <<<'CODE'
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'SNI_server_certs' => [
            "cs.php.net" => __DIR__ . "/sni_server_cs_expired.pem",
        ]
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $serverFlags, $ctx);
    phpt_notify_server_start($server);

    $conn = stream_socket_accept($server, 10);
    if ($conn) {
        fclose($conn);
    }

    phpt_wait();
CODE;

$clientCode = <<<'CODE'
    $clientCtx = stream_context_create([
        'ssl' => [
            'cafile' => __DIR__ . '/sni_server_ca.pem',
            'verify_peer' => true,
            'verify_peer_name' => true,
        ],
        "http" => [
            "proxy" => "tcp://{{ ADDR }}"
        ],
    ]);

    var_dump(@file_get_contents("https://cs.php.net/", false, $clientCtx));

    phpt_notify();
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
bool(false)
