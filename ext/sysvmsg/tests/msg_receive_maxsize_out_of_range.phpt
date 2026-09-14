--TEST--
msg_receive() must reject a maximum message size that exceeds a system size
--EXTENSIONS--
sysvmsg
--SKIPIF--
<?php
if (PHP_SYS_SIZE !== 4) die('skip only for a 4 byte system word');
if (PHP_INT_SIZE <= PHP_SYS_SIZE) die('skip only where a PHP integer is wider than a system size_t');
?>
--FILE--
<?php
$queue = msg_get_queue(0, 0600);

try {
    try {
        msg_receive($queue, 1, $received_type, PHP_INT_MAX, $message);
    } catch (ValueError $exception) {
        echo $exception::class, ': ', $exception->getMessage(), "\n";
    }
} finally {
    var_dump(msg_remove_queue($queue));
}
?>
--EXPECT--
ValueError: msg_receive(): Argument #4 ($max_message_size) must be less than or equal to 4294967295
bool(true)
