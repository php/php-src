--TEST--
Block Pass 006: Inorrect QM_ASSIGN elimination
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
match(match(''){c=>''}){}
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "c" in %sblock_pass_006.php:2
Stack trace:
#0 {main}
  thrown in %sblock_pass_006.php on line 2
