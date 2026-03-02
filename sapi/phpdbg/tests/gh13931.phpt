--TEST--
Applying zero offset to null pointer in Zend/zend_opcode.c
--FILE--
<?php
function foo () {
    try {
        break;
    } finally {
    }
}
foo();
?>
--PHPDBG--
ev 1 + 3
ev 2 ** 3
q
--EXPECTF--
Fatal error: 'break' not in the 'loop' or 'switch' context in %s on line %d
prompt> 4
prompt> 8
prompt>
