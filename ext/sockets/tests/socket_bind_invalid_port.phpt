--TEST--
socket_bind() with invalid ports.
--EXTENSIONS--
sockets
--FILE--
<?php
    $s_c     = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

    try {
    	socket_bind($s_c, '0.0.0.0', -1);
    } catch (\ValueError $e) {
	    echo $e->getMessage() . PHP_EOL;
    }

    try {
    	socket_bind($s_c, '0.0.0.0', 65536);
    } catch (\ValueError $e) {
	    echo $e->getMessage() . PHP_EOL;
    }
?>
--EXPECT--
socket_bind(): Argument #3 ($port) must be between 0 and 65535
socket_bind(): Argument #3 ($port) must be between 0 and 65535
