--TEST--
socket_addrinfo_lookup(): an empty packed hints array is accepted like []
--EXTENSIONS--
sockets
--FILE--
<?php
$hints = [1];
unset($hints[0]);
var_dump(count($hints), $hints === []);
var_dump(count(socket_addrinfo_lookup('127.0.0.1', '80', $hints)) === count(socket_addrinfo_lookup('127.0.0.1', '80', [])));
try {
    socket_addrinfo_lookup('127.0.0.1', '80', [1]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
int(0)
bool(true)
bool(true)
ValueError: socket_addrinfo_lookup(): Argument #3 ($hints) must only contain array keys "ai_flags", "ai_socktype", "ai_protocol", or "ai_family"
