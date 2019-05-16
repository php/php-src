--TEST--
Test socket_addrinfo_bind()
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('SKIP The sockets extension is not loaded.');
}
--FILE--
<?php
$addrinfo = socket_addrinfo_lookup('127.0.0.1', 2000, array(
    'ai_family' => AF_INET,
    'ai_socktype' => SOCK_DGRAM,
));
var_dump(socket_addrinfo_bind($addrinfo[0]));
echo "Done";
--EXPECTF--
resource(%d) of type (Socket)
Done
