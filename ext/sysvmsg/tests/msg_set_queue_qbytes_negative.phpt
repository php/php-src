--TEST--
msg_set_queue() must reject a negative msg_qbytes
--EXTENSIONS--
sysvmsg
--FILE--
<?php
$queue = msg_get_queue(0, 0600);

try {
    $stat = msg_stat_queue($queue);

    foreach ([-1, PHP_INT_MIN] as $value) {
        try {
            msg_set_queue($queue, ['msg_qbytes' => $value]);
        } catch (ValueError $exception) {
            echo $exception::class, ': ', $exception->getMessage(), "\n";
        }
    }

    var_dump($stat === msg_stat_queue($queue));
} finally {
    var_dump(msg_remove_queue($queue));
}
?>
--EXPECT--
ValueError: msg_set_queue(): Argument #2 ($data) "msg_qbytes" must be greater than or equal to 0
ValueError: msg_set_queue(): Argument #2 ($data) "msg_qbytes" must be greater than or equal to 0
bool(true)
bool(true)
