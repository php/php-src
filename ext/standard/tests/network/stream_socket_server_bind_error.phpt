--TEST--
stream_socket_server() bind error
--FILE--
<?php

$server1 = stream_socket_server("tcp://0.0.0.0:0");
$name = stream_socket_get_name($server1, false);
$server2 = stream_socket_server("tcp://$name");
fclose($server1);

?>
--EXPECTF--
Warning: stream_socket_server(): Unable to connect to tcp://0.0.0.0:%d (Address already in use) in %s on line %d
