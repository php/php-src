--TEST--
msg_send() must reject a message type outside the system message type range
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
            msg_send($queue, $type, 'hello');
        } catch (ValueError $exception) {
            echo $exception::class, ': ', $exception->getMessage(), "\n";
        }
    }
} finally {
    var_dump(msg_remove_queue($queue));
}
?>
--EXPECT--
ValueError: msg_send(): Argument #2 ($message_type) must be between -2147483648 and 2147483647
ValueError: msg_send(): Argument #2 ($message_type) must be between -2147483648 and 2147483647
bool(true)
