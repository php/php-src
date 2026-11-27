--TEST--
stream_socket_client(): JIT must not free a persistent stream returned twice
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
opcache.file_update_protection=0
--FILE--
<?php
function connect_twice(string $address): void
{
    $keep = stream_socket_client("tcp://$address", $errno, $errstr, 1, STREAM_CLIENT_PERSISTENT | STREAM_CLIENT_CONNECT);
    stream_socket_client("tcp://$address", $errno, $errstr, 1, STREAM_CLIENT_PERSISTENT | STREAM_CLIENT_CONNECT);
    var_dump(is_resource($keep));
}

$server = stream_socket_server('tcp://127.0.0.1:0');
connect_twice(stream_socket_get_name($server, false));
?>
--EXPECT--
bool(true)
