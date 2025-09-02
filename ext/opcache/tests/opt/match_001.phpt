--TEST--
Match 001: memory leak because of live range lose
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
+$y . +$y . match(y) {}
?>
--EXPECTF--
Warning: Undefined variable $y in %smatch_001.php on line 2

Warning: Undefined variable $y in %smatch_001.php on line 2

Fatal error: Uncaught Error: Undefined constant "y" in %smatch_001.php:2
Stack trace:
#0 {main}
  thrown in %smatch_001.php on line 2
