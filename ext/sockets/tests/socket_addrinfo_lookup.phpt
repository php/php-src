--TEST--
Test socket_addrinfo_lookup()
--EXTENSIONS--
sockets
--FILE--
<?php
try {
    $addrinfo = socket_addrinfo_lookup('127.0.0.1', 2000, array(
        'ai_family' => AF_INET,
        'ai_socktype' => SOCK_DGRAM,
        'invalid' => null,
    ));
    var_dump($addrinfo[0]);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
socket_addrinfo_lookup(): Argument #3 ($hints) must only contain array keys "ai_flags", "ai_socktype", "ai_protocol", or "ai_family"
