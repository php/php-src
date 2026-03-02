--TEST--
Match 003: SSA integrity verification failed because of incorrect optimization
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
match(y()){};
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function y() in %smatch_003.php:2
Stack trace:
#0 {main}
  thrown in %smatch_003.php on line 2
