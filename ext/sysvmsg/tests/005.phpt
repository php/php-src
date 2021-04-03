--TEST--
sysvmsg functions on non-existing queue
--EXTENSIONS--
sysvmsg
--FILE--
<?php

$tests = array(null, 'foo');

foreach ($tests as $i => $q) {

    if ($q === null) {
        do {
            $id = ftok(__FILE__, chr(mt_rand(0, 255))); } while (msg_queue_exists($id));
    }

    $q = msg_get_queue($id) or die("Failed to create queue");
    msg_remove_queue($q) or die("Failed to close queue");

    echo "Iteration " . ($i + 1) . ":\n";

    $errno = 0;

    var_dump(msg_set_queue($q, array('msg_qbytes' => 1)));

    var_dump(msg_stat_queue($q));

    var_dump(msg_receive($q, 0, $null, 1, $msg, true, 0, $errno));
    var_dump($errno != 0);
    // again, but triggering an exception
    try {
        msg_receive($q, 0, $null, 0, $msg);
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }

    var_dump(msg_send($q, 1, 'foo', true, true, $errno));
    var_dump($errno != 0);
}

echo "Done\n";
?>
--EXPECTF--
Iteration 1:
bool(false)
bool(false)
bool(false)
bool(true)
msg_receive(): Argument #4 ($max_message_size) must be greater than 0

Warning: msg_send(): msgsnd failed: Invalid argument in %s on line %d
bool(false)
bool(true)
Iteration 2:
bool(false)
bool(false)
bool(false)
bool(true)
msg_receive(): Argument #4 ($max_message_size) must be greater than 0

Warning: msg_send(): msgsnd failed: Invalid argument in %s on line %d
bool(false)
bool(true)
Done
