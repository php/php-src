--TEST--
GH-23879 (stream_socket_accept() reports AcceptFailed when a non-blocking listener has no pending connection)
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
try {
    stream_socket_accept($server, 0);
} catch (StreamException $e) {
    var_dump($e->getErrors()[0]->code);
}
?>
--EXPECT--
bool(false)
array(0) {
}
bool(true)
bool(false)
enum(StreamErrorCode::AcceptFailed)
