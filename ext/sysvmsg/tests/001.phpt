--TEST--
send/receive serialized message.
--SKIPIF--
<?php
if (!extension_loaded("sysvmsg")) print "skip"; ?>
--FILE--
<?php
$key = ftok(__DIR__ . "/001.phpt", "p");
$q = msg_get_queue($key);
msg_send($q, 1, "hello") or print "FAIL\n";
$type = null;
if (msg_receive($q, 0, $type, 1024, $message)) {
    echo "TYPE: $type\n";
    echo "DATA: $message\n";
}
if (!msg_remove_queue($q)) {
    echo "BAD: queue removal failed\n";
}
?>
--EXPECT--
TYPE: 1
DATA: hello
