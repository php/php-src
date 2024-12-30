--TEST--
socket_addrinfo_lookup with invalid hints
--EXTENSIONS--
sockets
--FILE--
<?php
try {
	socket_addrinfo_lookup('127.0.0.1', 2000, array(
		'ai_family' => new stdClass(),
		'ai_socktype' => SOCK_DGRAM,
		'ai_flags' => 0,
		'ai_protocol' => 0,
	));
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_addrinfo_lookup('127.0.0.1', 2000, array(
		'ai_family' => AF_INET,
		'ai_socktype' => new stdClass(),
		'ai_flags' => 0,
		'ai_protocol' => 0,
	));
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_addrinfo_lookup('127.0.0.1', 2000, array(
		'ai_family' => AF_INET,
		'ai_socktype' => SOCK_DGRAM,
		'ai_flags' => new stdClass(),
		'ai_protocol' => 0,
	));
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_addrinfo_lookup('127.0.0.1', 2000, array(
		'ai_family' => AF_INET,
		'ai_socktype' => SOCK_DGRAM,
		'ai_flags' => 0,
		'ai_protocol' => new stdClass(),
	));
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECT--
socket_addrinfo_lookup(): Argument #3 ($hints) "ai_family" key must be of type int, stdClass given
socket_addrinfo_lookup(): Argument #3 ($hints) "ai_socktype" key must be of type int, stdClass given
socket_addrinfo_lookup(): Argument #3 ($hints) "ai_flags" key must be of type int, stdClass given
socket_addrinfo_lookup(): Argument #3 ($hints) "ai_protocol" key must be of type int, stdClass given
