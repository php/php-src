--TEST--
PSK callbacks fail when the SSL library does not support PSK
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!str_contains(OPENSSL_VERSION_TEXT, 'LibreSSL')) die("skip only for SSL libraries without PSK support");
?>
--FILE--
<?php
function try_psk(string $option, bool $server): void {
    $listener = stream_socket_server('tcp://127.0.0.1:0');
    $client = stream_socket_client('tcp://' . stream_socket_get_name($listener, false));
    $peer = stream_socket_accept($listener);
    $stream = $server ? $peer : $client;
    stream_context_set_option($stream, 'ssl', $option, fn() => null);
    stream_set_blocking($stream, false);
    var_dump(stream_socket_enable_crypto($stream, true,
        $server ? STREAM_CRYPTO_METHOD_TLS_SERVER : STREAM_CRYPTO_METHOD_TLS_CLIENT));
}

try_psk('psk_client_cb', false);
try_psk('psk_server_cb', true);
?>
--EXPECTF--
Warning: stream_socket_enable_crypto(): psk_client_cb is not supported by the SSL library against which PHP is linked in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): psk_server_cb is not supported by the SSL library against which PHP is linked in %s on line %d
bool(false)
