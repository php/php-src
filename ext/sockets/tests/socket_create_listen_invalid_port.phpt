--TEST--
socket_create_listen() using invalid ports
--EXTENSIONS--
sockets
--FILE--
<?php
var_dump(socket_create_listen(0));

try {
	socket_create_listen(-1);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_create_listen(65536);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECT--
object(Socket)#1 (0) {
}
socket_create_listen(): Argument #1 ($port) must be between 0 and 65535
socket_create_listen(): Argument #1 ($port) must be between 0 and 65535
