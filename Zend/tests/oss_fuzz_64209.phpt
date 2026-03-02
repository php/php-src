--TEST--
oss-fuzz #64209: Fix in-place modification of filename in php_message_handler_for_zend
--FILE--
<?php
require '://@';
?>
--EXPECTF--
Warning: require(://@): Failed to open stream: No such file or directory in %s on line %d

Fatal error: Uncaught Error: Failed opening required '://@' (include_path='%s') in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
