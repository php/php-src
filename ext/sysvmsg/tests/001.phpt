--TEST--
Check for sysvmsg presence
--SKIPIF--
<?php // vim600:syn=php
if (!extension_loaded("sysvmsg")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php 
$q = msg_get_queue(911);
msg_send($q, 1, "hello") or print "FAIL\n";
$type = null;
$data = msg_receive($q, 0, $type, 1024);
if ($data !== false) {
	echo "TYPE: $type\n";
	echo "DATA: $data\n";
}
if (!msg_remove_queue($q)) {
	echo "BAD: queue removal failed\n";
}
?>
--EXPECT--
TYPE: 1
DATA: hello
