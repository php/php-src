--TEST--
jmp into a finally block 03
--FILE--
<?php
function foo() {
	try {
    } finally {
	goto test;
test:
    }
}
echo "okey";
?>
--EXPECTF--
okey
