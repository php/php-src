--TEST--
openssl_get_channel_binding(): TLS 1.2 PSK, tls-server-end-point is not applicable
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
    $conn = stream_socket_accept($server, 30);
    fwrite($conn, "ready");
    phpt_wait();
    fclose($conn);
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

    foreach (Openssl\ChannelBindingType::cases() as $type) {
        try {
            $v = openssl_get_channel_binding($client, $type);
            printf("%s len=%d\n", $type->value, strlen($v));
        } catch (Openssl\UnsupportedChannelBindingException $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }

    phpt_notify('server');
    fclose($client);
CODE;

include __DIR__ . '/ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECTF--
bool(true)
string(5) "ready"
tls-unique len=%d
Openssl\UnsupportedChannelBindingException: Channel binding type "tls-server-end-point" requires a certificate
tls-exporter len=32
