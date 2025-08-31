--TEST--
GH-8472: The resource returned by stream_socket_accept may have incorrect metadata
--FILE--
<?php
function setNonBlocking($stream)
{
    $block = stream_get_meta_data($stream)['blocked'];
    if ($block) {
        stream_set_blocking($stream, false);
    }
}

$server = stream_socket_server("tcp://127.0.0.1:9100");
setNonBlocking($server);

$client = stream_socket_client("tcp://127.0.0.1:9100");

$res = stream_socket_accept($server);
stream_set_timeout($res, 1);
setNonBlocking($res);

fwrite($client, str_repeat('0', 5));

$read = [$res];
$write = [];
$except = [];

if (stream_select($read, $write, $except, 1)) {
    var_dump(fread($res, 4));
    var_dump(fread($res, 4));
}
?>
--EXPECT--
string(4) "0000"
string(1) "0"
