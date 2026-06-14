--TEST--
TLS 1.2 PSK: mismatching key material aborts handshake
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$serverCode = <<<'CODE'
    $serverCtx = stream_context_create(['ssl' => [
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_SERVER,
        'ciphers' => 'PSK',
        'psk_server_cb' => function ($stream, string $identity): ?Openssl\Psk {
            return new Openssl\Psk(str_repeat("\x11", 16), $identity);
        },
    ]]);
    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $serverCtx);
    phpt_notify_server_start($server);
    @stream_socket_accept($server, 3);
CODE;

$clientCode = <<<'CODE'
    $clientCtx = stream_context_create(['ssl' => [
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT,
        'ciphers' => 'PSK',
        'verify_peer' => false,
        'psk_client_cb' => function ($stream): ?Openssl\Psk {
            return new Openssl\Psk(str_repeat("\x22", 16), 'id');
        },
    ]]);
    $client = @stream_socket_client('tls://{{ ADDR }}', $errno, $errstr,
        5, STREAM_CLIENT_CONNECT, $clientCtx);
    var_dump($client);
CODE;

include __DIR__ . '/ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
bool(false)

