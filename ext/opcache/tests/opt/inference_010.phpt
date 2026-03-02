--TEST--
Type inference 010: FRTCH_DIM_W
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function foo() {
    $x = false;
    $x[] = &$y;
}
foo();
?>
DONE
--EXPECTF--
Deprecated: Automatic conversion of false to array is deprecated in %sinference_010.php on line 4
DONE
