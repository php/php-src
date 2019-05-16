--TEST--
nullable class
--FILE--
<?php
function test(Foo $a = null) {
	echo "ok\n";
}
test(null);
?>
--EXPECT--
ok
