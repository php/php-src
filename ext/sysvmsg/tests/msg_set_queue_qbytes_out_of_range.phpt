--TEST--
msg_set_queue() must reject a msg_qbytes that exceeds the system range
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
    $stat = msg_stat_queue($queue);

    /* Truncated to the system width this leaves 16384, a valid queue size, so
     * accepting it would leave no sign of the lost bits. */
    try {
        msg_set_queue($queue, ['msg_qbytes' => 0x100004000]);
    } catch (ValueError $exception) {
        echo $exception::class, ': ', $exception->getMessage(), "\n";
    }

    var_dump($stat === msg_stat_queue($queue));
} finally {
    var_dump(msg_remove_queue($queue));
}
?>
--EXPECT--
ValueError: msg_set_queue(): Argument #2 ($data) "msg_qbytes" must be less than or equal to 4294967295
bool(true)
bool(true)
