--TEST--
msg_receive() must reject a desired message type outside the system message type range
--EXTENSIONS--
sysvmsg
--SKIPIF--
<?php
if (PHP_SYS_SIZE !== 4) die('skip only for a 4 byte system word');
if (PHP_INT_SIZE <= PHP_SYS_SIZE) die('skip only where a PHP integer is wider than a system long');
?>
--FILE--
<?php
$queue = msg_get_queue(0, 0600);

try {
    foreach ([0x100000000, -0x100000000] as $type) {
        try {
            msg_receive($queue, $type, $received_type, 1024, $message);
        } catch (ValueError $exception) {
            echo $exception::class, ': ', $exception->getMessage(), "\n";
        }
    }
} finally {
    var_dump(msg_remove_queue($queue));
}
?>
--EXPECT--
ValueError: msg_receive(): Argument #2 ($desired_message_type) must be between -2147483648 and 2147483647
ValueError: msg_receive(): Argument #2 ($desired_message_type) must be between -2147483648 and 2147483647
bool(true)
