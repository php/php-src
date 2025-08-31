--TEST--
GH-10885: stream_socket_server context leaks
--FILE--
<?php
$context = stream_context_create();
debug_zval_dump($context);
$server = @\stream_socket_server(
    'tcp://127.0.0.1:0',
    $errno,
    $errstr,
    \STREAM_SERVER_BIND | \STREAM_SERVER_LISTEN,
    $context,
);
debug_zval_dump($context);
fclose($server);
unset($server);
debug_zval_dump($context);
?>
--EXPECTF--
resource(%d) of type (stream-context) refcount(2)
resource(%d) of type (stream-context) refcount(3)
resource(%d) of type (stream-context) refcount(2)
