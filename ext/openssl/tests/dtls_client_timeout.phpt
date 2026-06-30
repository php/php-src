--TEST--
dtls:// client: the handshake is bounded by the connect timeout
--EXTENSIONS--
openssl
--SKIPIF--
<?php require __DIR__ . '/dtls_skipif.inc'; ?>
--FILE--
<?php
// A bound UDP socket that never speaks DTLS: the ClientHello is retransmitted but
// never answered, so the handshake must give up at the connect timeout instead of
// retransmitting forever.
$sink = stream_socket_server('udp://127.0.0.1:0', $errno, $errstr, STREAM_SERVER_BIND);
$port = (int) substr(strrchr(stream_socket_get_name($sink, false), ':'), 1);

$start = microtime(true);
$client = @stream_socket_client("dtls://127.0.0.1:$port", $errno, $errstr, 2,
    STREAM_CLIENT_CONNECT, stream_context_create(['ssl' => ['verify_peer' => false]]));
$elapsed = microtime(true) - $start;

var_dump($client === false);
var_dump($elapsed >= 1 && $elapsed < 5);

fclose($sink);
?>
--EXPECT--
bool(true)
bool(true)
