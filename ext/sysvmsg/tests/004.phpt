--TEST--
msg_set_queue() and msg_stat_queue()
--SKIPIF--
<?php if (!extension_loaded("sysvmsg")) die("skip sysvmsg extension is not available")?>
--FILE--
<?php
$id = ftok(__FILE__, 'r');

$q = msg_get_queue($id);

echo "Set mode:\n";
$arr = array('msg_perm.mode' => 0600);
var_dump(msg_set_queue($q, $arr));
echo "Did really work:\n";
var_dump(count(array_diff_assoc($arr, msg_stat_queue($q))) == 0);

echo "Set uid:\n"; // same as the running user to make it succeed
$arr = array('msg_perm.uid' => getmyuid());
var_dump(msg_set_queue($q, $arr));
echo "Did really work:\n";
var_dump(count(array_diff_assoc($arr, msg_stat_queue($q))) == 0);

echo "Set gid:\n"; // same as the running user to make it succeed
$arr = array('msg_perm.gid' => getmygid());
var_dump(msg_set_queue($q, $arr));
echo "Did really work:\n";
var_dump(count(array_diff_assoc($arr, msg_stat_queue($q))) == 0);

echo "Set smaller qbytes:\n";
$res = msg_stat_queue($q);
$arr = array('msg_qbytes' => ($res['msg_qbytes'] -1));
var_dump(msg_set_queue($q, $arr));
echo "Did really work:\n";
var_dump(count(array_diff_assoc($arr, msg_stat_queue($q))) == 0);

if (!msg_remove_queue($q)) {
        echo "BAD: queue removal failed\n";
}

echo "Done\n";
?>
--EXPECTF--
Set mode:
bool(true)
Did really work:
bool(true)
Set uid:
bool(true)
Did really work:
bool(true)
Set gid:
bool(true)
Did really work:
bool(true)
Set smaller qbytes:
bool(true)
Did really work:
bool(true)
Done
