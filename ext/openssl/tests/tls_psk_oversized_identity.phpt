--TEST--
TLS PSK - oversized identity from an unserialized Psk is rejected
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
if (OPENSSL_VERSION_NUMBER < 0x10101000) die("skip OpenSSL >= 1.1.1 required");
?>
--FILE--
<?php
$serverCode = <<<'CODE'
    $serverCtx = stream_context_create(['ssl' => [
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_SERVER,
        'ciphers' => 'PSK',
        'psk_server_cb' => function ($stream, string $identity): ?Openssl\Psk {
            return new Openssl\Psk(str_repeat("\x42", 16));
        },
    ]]);
    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $serverCtx);
    phpt_notify_server_start($server);
    @stream_socket_accept($server, 3);
CODE;

$clientCode = <<<'CODE'
    // unserialize bypasses the constructor length check, so the Psk carries a
    // 512-byte identity; the callback must reject it, not overflow the buffer.
    $identity = str_repeat('A', 512);
    $payload = sprintf('O:11:"Openssl\Psk":2:{s:3:"psk";s:4:"pass";s:8:"identity";s:%d:"%s";}',
        strlen($identity), $identity);

    $clientCtx = stream_context_create(['ssl' => [
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT,
        'ciphers' => 'PSK',
        'verify_peer' => false,
        'verify_peer_name' => false,
        'psk_client_cb' => function ($stream) use ($payload): ?Openssl\Psk {
            return unserialize($payload);
        },
    ]]);
    try {
        @stream_socket_client('tls://{{ ADDR }}', $errno, $errstr,
            30, STREAM_CLIENT_CONNECT, $clientCtx);
        echo "no exception\n";
    } catch (ValueError $e) {
        echo "caught: ", $e->getMessage(), "\n";
    }
CODE;

include __DIR__ . '/ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECTF--
caught: PSK identity must be shorter than %d bytes
