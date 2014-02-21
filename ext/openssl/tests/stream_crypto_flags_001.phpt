--TEST--
Basic bitwise stream crypto context flag assignment
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
if (!function_exists('pcntl_fork')) die("skip no fork");
--FILE--
<?php
$flags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
$ctx = stream_context_create(['ssl' => [
    'local_cert' => __DIR__ . '/bug54992.pem',
    'allow_self_signed' => true
]]);
$server = stream_socket_server('ssl://127.0.0.1:64321', $errno, $errstr, $flags, $ctx);
var_dump($server);

$pid = pcntl_fork();
if ($pid == -1) {
    die('could not fork');
} else if ($pid) {

    // Base SSL context values
    $sslCtxVars = array(
        'verify_peer' => TRUE,
        'cafile' => __DIR__ . '/bug54992-ca.pem',
        'CN_match' => 'bug54992.local', // common name from the server's "local_cert" PEM file
    );

    // SSLv3
    $ctxCopy = $sslCtxVars;
    $ctxCopy['crypto_method'] = STREAM_CRYPTO_METHOD_SSLv3_CLIENT;
    $ctx = stream_context_create(array('ssl' => $ctxCopy));
    var_dump(stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 1, STREAM_CLIENT_CONNECT, $ctx));

    // TLSv1
    $ctxCopy = $sslCtxVars;
    $ctxCopy['crypto_method'] = STREAM_CRYPTO_METHOD_TLSv1_0_CLIENT;
    $ctx = stream_context_create(array('ssl' => $ctxCopy));
    var_dump(stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 1, STREAM_CLIENT_CONNECT, $ctx));

    // TLS (any)
    $ctxCopy = $sslCtxVars;
    $ctxCopy['crypto_method'] = STREAM_CRYPTO_METHOD_TLS_CLIENT;
    $ctx = stream_context_create(array('ssl' => $ctxCopy));
    var_dump(stream_socket_client("ssl://127.0.0.1:64321", $errno, $errstr, 1, STREAM_CLIENT_CONNECT, $ctx));

} else {
    @pcntl_wait($status);
    @stream_socket_accept($server, 1);
    @stream_socket_accept($server, 1);
    @stream_socket_accept($server, 1);
}
--EXPECTF--
resource(%d) of type (stream)
resource(%d) of type (stream)
resource(%d) of type (stream)
resource(%d) of type (stream)

