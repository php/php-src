--TEST--
GH-24173 (Timed writes on socket streams must not block past the timeout)
--FILE--
<?php
$srv = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
$client = stream_socket_client('tcp://' . stream_socket_get_name($srv, false), $errno, $errstr, 5);
$conn = stream_socket_accept($srv);

// Neither side reads, so writes eventually fill the socket buffers. A write
// that times out may still have written part of the chunk, so stop at the
// first timeout rather than at the first zero-length write.
$chunk = str_repeat('x', 1 << 20);
foreach (['client' => $client, 'accepted' => $conn] as $name => $stream) {
    stream_set_timeout($stream, 0, 200000);
    for ($i = 0; $i < 256; $i++) {
        @fwrite($stream, $chunk);
        if (stream_get_meta_data($stream)['timed_out']) {
            break;
        }
    }
    echo $name, ': ';
    var_dump(stream_get_meta_data($stream)['timed_out']);
}
?>
--EXPECT--
client: bool(true)
accepted: bool(true)
