--TEST--
Test socket_addrinfo_lookup()
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
    'invalid' => null,
));
var_dump($addrinfo[0]);
echo "Done";
--EXPECTF--
Notice: socket_addrinfo_lookup(): Unknown hint invalid in %ssocket_addrinfo_lookup.php on line %d
resource(%d) of type (AddressInfo)
Done
