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
try {
	socket_addrinfo_lookup('127.0.0.1', 2000, array(
		'ai_family' => PHP_INT_MAX,
		'ai_socktype' => SOCK_DGRAM,
		'ai_flags' => 0,
		'ai_protocol' => 0,
	));
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_addrinfo_lookup('127.0.0.1', 2000, array(
		'ai_family' => AF_INET,
		'ai_socktype' => -1,
		'ai_flags' => 0,
		'ai_protocol' => 0,
	));
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_addrinfo_lookup('127.0.0.1', 2000, array(
		'ai_family' => AF_INET,
		'ai_socktype' => SOCK_DGRAM,
		'ai_flags' => -256,
		'ai_protocol' => 0,
	));
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_addrinfo_lookup('127.0.0.1', 2000, array(
		'ai_family' => AF_INET,
		'ai_socktype' => SOCK_DGRAM,
		'ai_flags' => 0,
		'ai_protocol' => PHP_INT_MIN,
	));
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_addrinfo_lookup('127.0.0.1', 2000, [
		AF_INET,
		SOCK_DGRAM,
		0,
		0,
	]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_addrinfo_lookup('127.0.0.1', 2000, array(
		'ai_family' => AF_INET,
		'ai_socktype' => SOCK_DGRAM,
		0,
		0,
	));
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECTF--
socket_addrinfo_lookup(): Argument #3 ($hints) "ai_family" key must be of type int, stdClass given
socket_addrinfo_lookup(): Argument #3 ($hints) "ai_socktype" key must be of type int, stdClass given
socket_addrinfo_lookup(): Argument #3 ($hints) "ai_flags" key must be of type int, stdClass given
socket_addrinfo_lookup(): Argument #3 ($hints) "ai_protocol" key must be of type int, stdClass given
socket_addrinfo_lookup(): Argument #3 ($hints) "ai_family" key must be AF_INET%A
socket_addrinfo_lookup(): Argument #3 ($hints) "ai_socktype" key must be between 0 and %d
socket_addrinfo_lookup(): Argument #3 ($hints) "ai_flags" key must be between 0 and %d
socket_addrinfo_lookup(): Argument #3 ($hints) "ai_protocol" key must be between 0 and %d
socket_addrinfo_lookup(): Argument #3 ($hints) must only contain array keys "ai_flags", "ai_socktype", "ai_protocol", or "ai_family"
socket_addrinfo_lookup(): Argument #3 ($hints) must only contain array keys "ai_flags", "ai_socktype", "ai_protocol", or "ai_family"
