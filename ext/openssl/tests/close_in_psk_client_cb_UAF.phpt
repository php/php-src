--TEST--
Closing the stream in the `psk_client_cb` callback triggers use after free
--EXTENSIONS--
openssl
--CREDITS--
Calif.io
--FILE--
<?php

$serverCode = <<<'PHP'
$context = stream_context_create(['ssl' => [
    'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_SERVER,
    'ciphers' => 'PSK',
    'psk_server_cb' => static function ($stream, string $identity): ?Openssl\Psk {
        if ($identity !== 'audit-client') {
            return null;
        }

        return new Openssl\Psk(str_repeat("\x41", 32));
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
    'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT,
    'ciphers' => 'PSK',
    'verify_peer' => false,
    'verify_peer_name' => false,
    'psk_client_cb' => static function ($stream): ?Openssl\Psk {
        fclose($stream);

        return new Openssl\Psk(str_repeat("\x41", 32), 'audit-client');
    },
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
