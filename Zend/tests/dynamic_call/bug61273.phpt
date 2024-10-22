--TEST--
Bug #61273 (call_user_func_array with more than 16333 arguments leaks / crashes)
--FILE--
<?php
/**
 * for 5.3 #define ZEND_VM_STACK_PAGE_SIZE ((64 * 1024) - 64)
 * for 5.4 #define ZEND_VM_STACK_PAGE_SIZE ((16 * 1024) - 16)
 * we should trick EG(argument_stack) into growing
 */
$args = array_fill(0, 64 * 1024 - 64, 0);
call_user_func_array(function(&$a) {}, $args);
echo strval("okey");
?>
--EXPECTF--
Warning: {closure:%s:%d}(): Argument #1 ($a) must be passed by reference, value given in %s on line %d
okey
