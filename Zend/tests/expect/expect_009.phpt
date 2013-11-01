--TEST--
test stack trace is correct from failed exception in extended class
--INI--
zend.expectations=1
--FILE--
<?php
class One {
    public function __construct() {
        
    }
}
class Two extends One {
    public function __construct() {
        expect false;
    }
}
new Two();
?>
--EXPECTF--	
Fatal error: Uncaught exception 'ExpectationException' with message 'expect false' in %s/expect_009.php:%d
Stack trace:
#0 %s/expect_009.php(%d): Two->__construct()
#1 {main}
  thrown in %s/expect_009.php on line %d
