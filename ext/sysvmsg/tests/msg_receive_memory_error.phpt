--TEST--
msg_receive() throws a catchable MemoryError when the maximum size cannot fit in the memory limit
--EXTENSIONS--
sysvmsg
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

$queue = msg_get_queue(ftok(__FILE__, 'r'));

try {
    msg_receive($queue, 0, $msgtype, PHP_INT_MAX - 100, $message, true, MSG_IPC_NOWAIT);
} catch (MemoryError $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

msg_send($queue, 1, 'ping');
var_dump(msg_receive($queue, 0, $msgtype, 1024, $message, true, MSG_IPC_NOWAIT));
var_dump($message);

msg_remove_queue($queue);

?>
--EXPECT--
MemoryError: The resulting string is too large to fit in the configured memory limit
bool(true)
string(4) "ping"
