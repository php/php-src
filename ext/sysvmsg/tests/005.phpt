--TEST--
sysvmsg functions on non-existing queue
--SKIPIF--
<?php if (!extension_loaded("sysvmsg")) die("skip sysvmsg extension is not available")?>
--FILE--
<?php

$tests = array(null, 'foo');

foreach ($tests as $q) {

    if ($q === null) {
	do {
	    $id = ftok(__FILE__, chr(mt_rand(0, 255)));
	} while (msg_queue_exists($id));

	$q = msg_get_queue($id) or die("Failed to create queue");
	msg_remove_queue($q) or die("Failed to close queue");
    }

    echo "Using '$q' as queue resource:\n";

    $errno = 0;

    var_dump(msg_set_queue($q, array('msg_qbytes' => 1)));

    var_dump(msg_stat_queue($q));

    var_dump(msg_receive($q, 0, $null, 1, $msg, true, 0, $errno));
    var_dump($errno != 0);
    // again, but triggering an E_WARNING
    var_dump(msg_receive($q, 0, $null, 0, $msg));

    var_dump(msg_send($q, 1, 'foo', true, true, $errno));
    var_dump($errno != 0);
}

echo "Done\n";
?>
--EXPECTF--
Using 'Resource id #4' as queue resource:
bool(false)
bool(false)
bool(false)
bool(true)

Warning: msg_receive(): maximum size of the message has to be greater than zero in %s on line %d
bool(false)

Warning: msg_send(): msgsnd failed: Invalid argument in %s on line %d
bool(false)
bool(true)
Using 'foo' as queue resource:

Warning: msg_set_queue() expects parameter 1 to be resource, string given in %s on line %d
bool(false)

Warning: msg_stat_queue() expects parameter 1 to be resource, string given in %s on line %d
bool(false)

Warning: msg_receive() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
bool(false)

Warning: msg_receive() expects parameter 1 to be resource, string given in %s on line %d
bool(false)

Warning: msg_send() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
bool(false)
Done
