--TEST--
COALESCE optimization
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function t() {
    $y[" $a "] ??= var_dump();
}
t();
?>
--EXPECTF--
Warning: Undefined variable $a in %scoalesce_002.php on line 3

Fatal error: Uncaught ArgumentCountError: var_dump() expects at least 1 argument, 0 given in %scoalesce_002.php:3
Stack trace:
#0 %scoalesce_002.php(3): var_dump()
#1 %scoalesce_002.php(5): t()
#2 {main}
  thrown in %scoalesce_002.php on line 3