--TEST--
test stack trace is correct from failed exception in extended class (parent implementing constructor)
--INI--
zend.assertions=1
--FILE--
<?php
class One {
    public function __construct() {
        assert(false);
    }
}
class Two extends One {}

new Two();
?>
--EXPECTF--	
Fatal error: Uncaught exception 'AssertionException' with message 'assert(false)' in %sexpect_010.php:%d
Stack trace:
#0 %sexpect_010.php(%d): One->__construct()
#1 {main}
  thrown in %sexpect_010.php on line %d
