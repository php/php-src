--TEST--
Bug #79783: Segfault in php_str_replace_common
--FILE--
<?php
str_replace("a", "b", "c", strlen("d"));
?>
--EXPECTF--
Fatal error: Uncaught Error: str_replace(): Argument #4 ($count) cannot be passed by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
