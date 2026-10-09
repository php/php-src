--TEST--
GH-24173 (Timed writes on socket streams must not block past the timeout)
--FILE--
<?php
$srv = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
$client = stream_socket_client('tcp://' . stream_socket_get_name($srv, false), $errno, $errstr, 5);
$conn = stream_socket_accept($srv);

// Neither side reads, so each write eventually fills the socket buffers.
$chunk = str_repeat('x', 1 << 20);
foreach (['client' => $client, 'accepted' => $conn] as $name => $stream) {
    stream_set_timeout($stream, 0, 200000);
    for ($i = 0; $i < 256 && @fwrite($stream, $chunk); $i++);
    echo $name, ': ';
    var_dump(stream_get_meta_data($stream)['timed_out']);
}
?>
--EXPECT--
client: bool(true)
accepted: bool(true)
