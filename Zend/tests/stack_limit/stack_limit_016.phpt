--TEST--
Stack limit 016 - Compilation of nested expressions uses little stack per nesting level
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('skip ASan uses considerably more stack per frame');
?>
--INI--
zend.max_allowed_stack_size=1M
--FILE--
<?php

/* Compiling an expression recurses once per nesting level, so every level may only use a small
 * amount of stack. When zend_compile_expr_inner() gets inlined into zend_compile_expr(), its frame
 * becomes part of each level of the recursion and the expression below no longer compiles. */

$a = 'x';

eval('$b = $a' . str_repeat(' . $a', 4000) . ';');

var_dump(strlen($b));

?>
--EXPECT--
int(4001)
