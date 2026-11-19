--TEST--
GH-25148 (Blocking socket fread() reports timed_out on Windows after returning buffered data)
--FILE--
<?php
$domain = PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX;
[$client, $server] = stream_socket_pair($domain, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);

fwrite($server, str_repeat('x', 1500));
usleep(100000);

// takes all 1500 bytes into the read buffer and returns the first 1024
var_dump(strlen(fread($client, 1024)));
var_dump(stream_get_meta_data($client)['timed_out']);

// returns the other 476 from the read buffer without waiting for more
var_dump(strlen(fread($client, 1024)));
var_dump(stream_get_meta_data($client)['timed_out']);
?>
--EXPECT--
int(1024)
bool(false)
int(476)
bool(false)
