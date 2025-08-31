--TEST--
Block Pass 005: Inorrect QM_ASSIGN elimination
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
y??(1?4:y)?->y;
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "y" in %sblock_pass_005.php:2
Stack trace:
#0 {main}
  thrown in %sblock_pass_005.php on line 2
