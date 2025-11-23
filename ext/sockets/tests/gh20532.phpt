--TEST--
GH-20562 - socket_addrinfo_lookup() returns error codes on resolution failures.
--EXTENSIONS--
sockets
--FILE--
<?php
var_dump(socket_addrinfo_lookup(".whynot") == EAI_NONAME);
var_dump(in_array(socket_addrinfo_lookup("2001:db8::1", null, ['ai_family' => AF_INET]), [EAI_FAMILY, EAI_ADDRFAMILY, EAI_NONAME, EAI_NODATA]));
var_dump(in_array(socket_addrinfo_lookup("example.com", "http", ['ai_socktype' => SOCK_RAW, 'ai_flags' => 2147483647]), [EAI_SOCKTYPE, EAI_SERVICE, EAI_BADFLAGS, EAI_NONAME]));
//var_dump(socket_addrinfo_lookup("2001:db8::1", null, ['ai_family' => AF_INET]));
//var_dump(socket_addrinfo_lookup("example.com", "http", ['ai_socktype' => SOCK_RAW, 'ai_flags' => 2147483647]));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)

