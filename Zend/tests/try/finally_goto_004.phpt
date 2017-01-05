--TEST--
jmp into a finally block 03
--FILE--
<?php
function foo() {
	try {
    } finally {
test:
    }
	goto test;
}
?>
--EXPECTF--
Fatal error: jump into a finally block is disallowed in %sfinally_goto_004.php on line %d
