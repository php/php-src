--TEST--
GH-20562 - socket_addrinfo_lookup() returns error codes on resolution failures.
--EXTENSIONS--
sockets
--FILE--
<?php
$error_code = 0;
var_dump(socket_addrinfo_lookup(".whynot", null, [], $error_code) === false && in_array($error_code, [EAI_NONAME, EAI_FAIL], true));
var_dump(socket_addrinfo_lookup("2001:db8::1", null, ['ai_family' => AF_INET], $error_code) === false && in_array($error_code, [EAI_FAMILY, EAI_ADDRFAMILY, EAI_NONAME, EAI_NODATA]));
var_dump(socket_addrinfo_lookup("example.com", "http", ['ai_socktype' => SOCK_RAW, 'ai_flags' => 2147483647], $error_code) === false && in_array($error_code, [EAI_SOCKTYPE, EAI_SERVICE, EAI_BADFLAGS, EAI_NONAME]));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)

