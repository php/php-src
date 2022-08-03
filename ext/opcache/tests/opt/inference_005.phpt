--TEST--
Type inference 005: Use MAY_BE_NULL result (insted of empty) for ASSIGN_DIM with invalid arguments
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function foo() {
    $a = $r[] = $r = [] & $y;
    +list(&$y) = $a;
}
?>
DONE
--EXPECT--
DONE
