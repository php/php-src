--TEST--
GH-25172: TLS session reuse must not share callbacks with a persistent stream
--EXTENSIONS--
openssl
--CREDITS--
cxxz16
--FILE--
<?php
function reuse_on_persistent(array $options, int $method): void {
    $server = stream_socket_server('tcp://127.0.0.1:0');
    $addr = stream_socket_get_name($server, false);
    [$host, $port] = explode(':', $addr);

    $ctx = stream_context_create(['ssl' => $options]);
    $parent = stream_socket_client("tcp://$addr", $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $ctx);
    stream_set_blocking($parent, false);
    var_dump(stream_socket_enable_crypto($parent, true, $method));

    $child = pfsockopen($host, (int) $port);
    stream_set_blocking($child, false);
    var_dump(stream_socket_enable_crypto($child, true, $method, $parent));
}

reuse_on_persistent(
    ['psk_client_cb' => fn() => new Openssl\Psk(str_repeat('k', 32), 'id')],
    STREAM_CRYPTO_METHOD_TLS_CLIENT,
);
reuse_on_persistent(
    ['session_new_cb' => function () {}],
    STREAM_CRYPTO_METHOD_TLS_CLIENT,
);
reuse_on_persistent(
    ['early_data_cb' => function () {}],
    STREAM_CRYPTO_METHOD_TLS_SERVER,
);
?>
--EXPECTF--
int(0)

Warning: stream_socket_enable_crypto(): Supplied SSL session stream with callbacks is not supported for persistent streams in %s on line %d
int(0)
int(0)

Warning: stream_socket_enable_crypto(): Supplied SSL session stream with callbacks is not supported for persistent streams in %s on line %d
int(0)
int(0)

Warning: stream_socket_enable_crypto(): Supplied SSL session stream with callbacks is not supported for persistent streams in %s on line %d
int(0)
