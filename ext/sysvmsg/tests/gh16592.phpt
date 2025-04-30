--TEST--
msg_send() segfault when the type does not serialize as expected
--EXTENSIONS--
sysvmsg
--FILE--
<?php
class Test {
    function __serialize() {}
}

$q = msg_get_queue(1);
try {
	msg_send($q, 1, new Test, true);
} catch (\TypeError $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
Test::__serialize() must return an array
