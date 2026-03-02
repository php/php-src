--TEST--
Match 002: memory leak because of incorrect optimization
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
match($y=y){};
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "y" in %smatch_002.php:2
Stack trace:
#0 {main}
  thrown in %smatch_002.php on line 2
