--TEST--
Setting an invalid TLS ALPN protocol list on a client stream fails
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (OPENSSL_VERSION_NUMBER < 0x30000000) die('skip For OpenSSL >= 3.0');
?>
--FILE--
<?php
$server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
$address = stream_socket_get_name($server, false);

function try_alpn($protocols, $streamOptions): void {
    global $address;

    $context = stream_context_create([
        'ssl' => ['alpn_protocols' => $protocols, 'verify_peer' => false],
        'stream' => $streamOptions,
    ]);
    $client = stream_socket_client("tcp://$address", $errno, $errstr, 1, STREAM_CLIENT_CONNECT, $context);
    var_dump(stream_socket_enable_crypto($client, true, STREAM_CRYPTO_METHOD_TLS_CLIENT));
    fclose($client);
}

foreach (['', ',', 'h2,', ',h2', 'h2,,http/1.1'] as $protocols) {
    try_alpn($protocols, []);
}

try_alpn('', []);
?>
--EXPECTF--
Warning: stream_socket_enable_crypto(): Failed setting TLS ALPN protocols, protocol names must not be empty in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): Failed setting TLS ALPN protocols, protocol names must not be empty in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): Failed setting TLS ALPN protocols, protocol names must not be empty in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): Failed setting TLS ALPN protocols, protocol names must not be empty in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): Failed setting TLS ALPN protocols, protocol names must not be empty in %s on line %d
bool(false)

Warning: stream_socket_enable_crypto(): Failed setting TLS ALPN protocols, protocol names must not be empty in %s on line %d
bool(false)
