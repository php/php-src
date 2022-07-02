--TEST--
Test socket_addrinfo_connect()
--EXTENSIONS--
sockets
--FILE--
<?php
$addrinfo = socket_addrinfo_lookup('127.0.0.1', 2000, array(
    'ai_family' => AF_INET,
    'ai_socktype' => SOCK_DGRAM,
));
var_dump(socket_addrinfo_connect($addrinfo[0]));
echo "Done";
?>
--EXPECT--
object(Socket)#2 (0) {
}
Done
