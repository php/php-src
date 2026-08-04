--TEST--
msg_send() segfault when the type does not serialize as expected
--EXTENSIONS--
sysvmsg
--FILE--
<?php
class Test {
    function __serialize() {}
}

// use a private queue, so we only remove our own.
$q = msg_get_queue(0, 0600);
try {
	msg_send($q, 1, new Test, true);
} catch (\TypeError $e) {
	echo $e->getMessage();
} finally {
	msg_remove_queue($q);
}
?>
--EXPECT--
Test::__serialize() must return an array
