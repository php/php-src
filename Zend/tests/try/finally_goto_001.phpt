--TEST--
jmp into a finally block 01
--FILE--
<?php
function foo() {
    goto test;
    try {
    } finally {
test:
    }
}
?>
--EXPECTF--
Fatal error: jump into a finally block is disallowed in %sfinally_goto_001.php on line %d
