--TEST--
socket_create_listen() using invalid ports
--EXTENSIONS--
sockets
--FILE--
<?php
var_dump(socket_create_listen(0));

try {
	socket_create_listen(-1);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
	socket_create_listen(65536);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
object(Socket)#1 (0) {
}
ValueError: socket_create_listen(): Argument #1 ($port) must be between 0 and 65535
ValueError: socket_create_listen(): Argument #1 ($port) must be between 0 and 65535
