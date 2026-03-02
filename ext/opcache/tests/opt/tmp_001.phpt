--TEST--
TMP variable optimization 001: TMP variable renumbering should be done after DFA optimizations
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
is_a((int)" $y " + 0);
?>
--EXPECTF--
Warning: Undefined variable $y in %stmp_001.php on line 2

Fatal error: Uncaught ArgumentCountError: is_a() expects at least 2 arguments, 1 given in %stmp_001.php:2
Stack trace:
#0 %stmp_001.php(2): is_a(0)
#1 {main}
  thrown in %stmp_001.php on line 2