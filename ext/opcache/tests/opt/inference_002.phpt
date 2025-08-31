--TEST--
Type inference 002: Type inference for INIT_ARRAY with invalid index
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
var_dump([[]=>&$x]);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot access offset of type array on array in %sinference_002.php:2
Stack trace:
#0 {main}
  thrown in %sinference_002.php on line 2
