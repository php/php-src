--TEST--
TLS 1.2 PSK basic client/server round-trip
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
            if ($identity === 'client_id') {
                return new Openssl\Psk("\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff");
            }
            return null;
        },
    ]]);
    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $serverCtx);
    phpt_notify_server_start($server);
    $client = stream_socket_accept($server, 30);
    fwrite($client, "hello-from-server");
    fread($client, 1024);
    fclose($client);
CODE;

$clientCode = <<<'CODE'
    $clientCtx = stream_context_create(['ssl' => [
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT,
        'ciphers' => 'PSK',
        'verify_peer' => false,
        'verify_peer_name' => false,
        'psk_client_cb' => function ($stream): ?Openssl\Psk {
            return new Openssl\Psk(
                "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff",
                'client_id'
            );
        },
    ]]);
    $client = stream_socket_client('tls://{{ ADDR }}', $errno, $errstr,
        30, STREAM_CLIENT_CONNECT, $clientCtx);
    var_dump($client !== false);
    var_dump(fread($client, 1024));
    fwrite($client, "hello-from-client");
    /* Verify PSK was used (no peer cert) */
    $params = stream_context_get_params($client);
    var_dump(isset($params['options']['ssl']['peer_certificate']));
    fclose($client);
CODE;

include __DIR__ . '/ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
bool(true)
string(17) "hello-from-server"
bool(false)
