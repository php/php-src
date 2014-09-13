--TEST--
Bug #54623: Segfault when when writing to a persistent socket after closing a copy of the socket
--FILE--
<?php
$sock = pfsockopen('udp://127.0.0.1', '63844');
var_dump((int)$sock);
@fwrite($sock, "1");
$sock2 = pfsockopen('udp://127.0.0.1', '63844');
var_dump((int)$sock2);
@fwrite($sock2, "2");
fclose($sock2);
fwrite($sock, "3");
--EXPECTF--
int(%d)
int(%d)

Warning: fwrite(): supplied resource is not a valid stream resource in %s on line %d
