--TEST--
Verify host name by default in client transfers
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
if (!function_exists('pcntl_fork')) die("skip no fork");
--FILE--
<?php
$serverUri = "ssl://127.0.0.1:64321";
$serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
$serverCtx = stream_context_create(['ssl' => [
    'local_cert' => __DIR__ . '/bug54992.pem'
]]);
$server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);

$pid = pcntl_fork();
if ($pid == -1) {
    die('could not fork');
} else if ($pid) {

    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'CN_match' => 'bug54992.local'
    ]]);

    $client = stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx);
    var_dump($client);

} else {
    @pcntl_wait($status);
    @stream_socket_accept($server, 1);
}
--EXPECTF--
resource(%d) of type (stream)
