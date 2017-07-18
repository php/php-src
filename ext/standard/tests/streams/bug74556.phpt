--TEST--
Bug #74556 stream_socket_get_name() on unix socket returns "\0"
--SKIPIF--
<?php
if (!strncasecmp(PHP_OS, 'WIN', 3)) echo "skip Unix Only";
--FILE--
<?php

$sock = tempnam(sys_get_temp_dir(), 'bug74556') . '.sock';
$s = stream_socket_server("unix://$sock");
$c = stream_socket_client("unix://$sock");

var_dump(
    stream_socket_get_name($s, true),
    stream_socket_get_name($c, false)
);
unlink($sock);
--EXPECT--
bool(false)
bool(false)
