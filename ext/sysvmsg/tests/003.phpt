--TEST--
msg_queue_exists()
--SKIPIF--
<?php if (!extension_loaded("sysvmsg")) die("skip sysvmsg extension is not available")?>
--FILE--
<?php
$id = ftok(__FILE__, 'r');

msg_remove_queue(msg_get_queue($id, 0600));

var_dump(msg_queue_exists($id));
$res = msg_get_queue($id, 0600);
var_dump($res);
var_dump(msg_queue_exists($id));
var_dump(msg_remove_queue($res));
var_dump(msg_queue_exists($id));
echo "Done\n";
?>
--EXPECT--
bool(false)
object(SysvMessageQueue)#1 (0) {
}
bool(true)
bool(true)
bool(false)
Done
