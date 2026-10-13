--TEST--
IO hooks: a provider on its own Poll context, timers included
--FILE--
<?php
include __DIR__ . '/context-provider.inc';

$provider = new ContextProvider();
\Io\Hooks\set_hooks($provider);

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);

$provider->spawn(function () use ($server) {
    $conn = stream_socket_accept($server, 5);
    usleep(20000);
    fwrite($conn, "hello\n");
    fclose($conn);
    echo "server done\n";
});
$provider->spawn(function () use ($addr) {
    $c = stream_socket_client("tcp://$addr");
    $line = trim(fgets($c));
    echo "client got: $line\n";
});
$provider->spawn(function () {
    usleep(5000);
    echo "slept\n";
});
$provider->spawn(function () use ($addr) {
    // A read that times out is a Timeout completion from the timer pair
    $c = stream_socket_client("tcp://$addr");
    stream_set_timeout($c, 0, 50000);
    var_dump(fgets($c));
    var_dump(stream_get_meta_data($c)['timed_out']);
});

$provider->loop();
echo "done\n";
?>
--EXPECT--
slept
server done
client got: hello
bool(false)
bool(true)
done
