--TEST--
GH-23879 (stream_socket_accept() reports AcceptFailed when no connection is pending)
--FILE--
<?php
$server = stream_socket_server(
    'tcp://127.0.0.1:0',
    context: stream_context_create([
        'stream' => [
            'error_mode' => StreamErrorMode::Exception,
            'error_store' => StreamErrorStore::All,
        ],
    ]),
);
stream_set_blocking($server, false);

var_dump(stream_socket_accept($server, 0));
var_dump(stream_last_errors());

$client = stream_socket_client('tcp://' . stream_socket_get_name($server, false));
var_dump(is_resource(stream_socket_accept($server, 1)));
var_dump(stream_socket_accept($server, 0));

stream_set_blocking($server, true);
var_dump(stream_socket_accept($server, 0));

try {
    stream_socket_accept($server, 0.1);
} catch (StreamException $e) {
    echo $e->getMessage(), "\n";
    echo $e->getErrors()[0]->code->name, "\n";
}
?>
--EXPECTF--
bool(false)
array(0) {
}
bool(true)
bool(false)
bool(false)
Accept failed: %s
AcceptFailed
