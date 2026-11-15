--TEST--
Closing the stream in the `psk_server_cb` callback triggers use after free
--EXTENSIONS--
openssl
--CREDITS--
Calif.io
--FILE--
<?php

$serverCode = <<<'PHP'
$context = stream_context_create(['ssl' => [
    'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_3_SERVER,
    'psk_server_cb' => function ($stream, string $identity): ?Openssl\Psk {
        fclose($stream);
        return null;
    },
]]);

$server = stream_socket_server(
    'tls://127.0.0.1:0',
    $errno,
    $errstr,
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN,
    $context,
);
phpt_notify_server_start($server);
@stream_socket_accept($server, 10);
PHP;

$clientCode = <<<'PHP'
$context = stream_context_create(['ssl' => [
    'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_3_CLIENT,
    'verify_peer' => false,
    'verify_peer_name' => false,
    'psk_client_cb' => fn ($stream) => new Openssl\Psk(
        str_repeat("\x42", 32),
        'remote-identity',
    ),
]]);

@stream_socket_client(
    'tls://{{ ADDR }}',
    $errno,
    $errstr,
    10,
    STREAM_CLIENT_CONNECT,
    $context,
);
PHP;

require 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

?>
--EXPECT--
