--TEST--
jmp into a finally block 02
--FILE--
<?php
function foo() {
	try {
		goto test;
    } finally {
test:
    }
}
?>
--EXPECTF--
Fatal error: jump into a finally block is disallowed in %sfinally_goto_002.php on line %d
