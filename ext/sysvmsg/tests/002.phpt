--TEST--
msg_receive() should return false when unserialize() failed
--SKIPIF--
<?php if (!extension_loaded("sysvmsg")) die("skip sysvmsg extenions is not available")?>
--FILE--
<?php

$queue = msg_get_queue (ftok(__FILE__, 'r'), 0600);
if (!msg_send ($queue, 1, 'Hi', false /* ! no_ser*/, true/*block*/, $msg_err)) {
	die("error\n");
}
var_dump($res = msg_receive ($queue, 1, $msg_type, 16384, $msg, true, 0, $msg_error));

if (!msg_remove_queue($queue)) {
	echo "BAD: queue removal failed\n";
}

echo "Done\n";
?>
--EXPECTF--
Warning: msg_receive(): message corrupted in %s002.php on line %d
bool(false)
Done
