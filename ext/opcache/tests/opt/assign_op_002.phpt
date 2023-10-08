--TEST--
ASSIGN_OP 002: Incorrect optimization of ASSIGN_OP may lead to incorrect result (sub assign -> pre dec conversion for null values)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function foo(int $a = null) {
    $a -= 1;
    return $a;
}
var_dump(foo(2));
var_dump(foo(null));
?>
--EXPECT--
int(1)
int(-1)
