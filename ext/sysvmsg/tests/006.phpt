--TEST--
msg_send() data types when not serializing
--SKIPIF--
<?php if (!extension_loaded("sysvmsg")) die("skip sysvmsg extenions is not available")?>
--FILE--
<?php

$queue = msg_get_queue (ftok(__FILE__, 'r'), 0600);

$tests = array('foo', 123, PHP_INT_MAX +1, true, 1.01, null, array('bar'));

foreach ($tests as $elem) {
    echo "Sending/receiving '$elem':\n";
    var_dump(msg_send($queue, 1, $elem, false));

    unset($msg);
    var_dump(msg_receive($queue, 1, $msg_type, 1024, $msg, false, MSG_IPC_NOWAIT));

    var_dump($elem == $msg);
    var_dump($elem === $msg);
}

if (!msg_remove_queue($queue)) {
	echo "BAD: queue removal failed\n";
}
	
echo "Done\n";
?>
--EXPECTF--
Sending/receiving 'foo':
bool(true)
bool(true)
bool(true)
bool(true)
Sending/receiving '123':
bool(true)
bool(true)
bool(true)
bool(false)
Sending/receiving '%d':
bool(true)
bool(true)
bool(true)
bool(false)
Sending/receiving '1':
bool(true)
bool(true)
bool(true)
bool(false)
Sending/receiving '1.01':
bool(true)
bool(true)
bool(true)
bool(false)
Sending/receiving '':

Warning: msg_send(): Message parameter must be either a string or a number. in %s on line %d
bool(false)
bool(false)
bool(true)
bool(false)
Sending/receiving 'Array':

Warning: msg_send(): Message parameter must be either a string or a number. in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)
Done
