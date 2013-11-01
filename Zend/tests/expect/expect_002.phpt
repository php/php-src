--TEST--
test failing expectation
--INI--
zend.expectations=1
--FILE--
<?php
expect false;
var_dump(true);
?>
--EXPECTF--	
Fatal error: Uncaught exception 'ExpectationException' with message 'expect false' in %s/expect_002.php:%d
Stack trace:
#0 {main}
  thrown in %s/expect_002.php on line %d

