--TEST--
stream_socket_enable_crypto() coerces the "crypto_method" context option to an integer
--EXTENSIONS--
openssl
--FILE--
<?php

function first_byte_sent_by_client(mixed $cryptoMethod): string
{
    $listener = stream_socket_server('tcp://127.0.0.1:0');
    $client = stream_socket_client('tcp://' . stream_socket_get_name($listener, false));
    $server = stream_socket_accept($listener);
    stream_set_blocking($client, false);
    stream_set_blocking($server, false);

    stream_context_set_option($client, 'ssl', 'crypto_method', $cryptoMethod);
    var_dump(stream_socket_enable_crypto($client, true));

    $read = [$server];
    $write = $except = null;
    stream_select($read, $write, $except, 1);

    return bin2hex(fread($server, 1));
}

/* 0x16 is the TLS handshake record type: the client sent a ClientHello. */
var_dump(first_byte_sent_by_client(STREAM_CRYPTO_METHOD_TLS_CLIENT));
var_dump(first_byte_sent_by_client((string) STREAM_CRYPTO_METHOD_TLS_CLIENT));
var_dump(first_byte_sent_by_client((float) STREAM_CRYPTO_METHOD_TLS_CLIENT));

?>
--EXPECT--
int(0)
string(2) "16"
int(0)
string(2) "16"
int(0)
string(2) "16"
