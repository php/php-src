--TEST--
oss-fuzz #64209: Fix in-place modification of filename
--INI--
opcache.enable_cli=1
--FILE--
<?php
require '://@';
?>
--EXPECTF--
Warning: require(://@): Failed to open stream: No such file or directory in %s on line %d

Fatal error: Uncaught Error: Failed opening required '://@' (include_path='.:') in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
