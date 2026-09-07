--TEST--
msg_queue_exists() and msg_get_queue() with IPC_PRIVATE
--EXTENSIONS--
sysvmsg
--FILE--
<?php
var_dump(msg_queue_exists(0));

$queue = msg_get_queue(0, 0600);

try {
    var_dump(msg_queue_exists(0));
    printf("%o\n", msg_stat_queue($queue)['msg_perm.mode']);
    var_dump(msg_send($queue, 1, 'hello'));
    var_dump(msg_receive($queue, 1, $type, 1024, $message));
    var_dump($message);
} finally {
    var_dump(msg_remove_queue($queue));
}
?>
--EXPECT--
bool(false)
bool(false)
600
bool(true)
bool(true)
string(5) "hello"
bool(true)
