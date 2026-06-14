--TEST--
Invalid opcode returned from zend_compile_var_inner() for memoized expression
--FILE--
<?php
strlen("foo")[0] ??= 123;
?>
--EXPECTF--
Fatal error: Cannot use result of built-in function in write context in %s on line %d
