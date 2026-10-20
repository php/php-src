--TEST--
TLS 1.3 PSK basic client/server round-trip
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
if (!defined('STREAM_CRYPTO_METHOD_TLSv1_3_SERVER')) die("skip TLS 1.3 not available");
?>
--FILE--
<?php
$serverCode = <<<'CODE'
    $serverCtx = stream_context_create(['ssl' => [
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_3_SERVER,
        'psk_server_cb' => function ($stream, string $identity): ?Openssl\Psk {
            if ($identity === 'tls13-client') {
                return new Openssl\Psk(str_repeat("\x42", 32));
            }
            return null;
        },
    ]]);
    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $serverCtx);
    phpt_notify_server_start($server);
    $client = stream_socket_accept($server, 30);
    fwrite($client, "tls13-psk-ok");
    fclose($client);
CODE;

$clientCode = <<<'CODE'
    $clientCtx = stream_context_create(['ssl' => [
        'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_3_CLIENT,
        'verify_peer' => false,
        'verify_peer_name' => false,
        'psk_client_cb' => function ($stream): ?Openssl\Psk {
            return new Openssl\Psk(str_repeat("\x42", 32), 'tls13-client');
        },
    ]]);
    $client = stream_socket_client('tls://{{ ADDR }}', $errno, $errstr,
        30, STREAM_CLIENT_CONNECT, $clientCtx);
    var_dump($client !== false);
    var_dump(fread($client, 1024));
    /* TLS 1.3 PSK: no peer cert */
    $params = stream_context_get_params($client);
    var_dump(isset($params['options']['ssl']['peer_certificate']));
    fclose($client);
CODE;

include __DIR__ . '/ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
bool(true)
string(12) "tls13-psk-ok"
bool(false)
