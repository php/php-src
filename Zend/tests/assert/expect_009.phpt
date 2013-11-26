--TEST--
test stack trace is correct from failed exception in extended class
--INI--
zend.assertions=1
--FILE--
<?php
class One {
    public function __construct() {
        
    }
}
class Two extends One {
    public function __construct() {
        assert(false);
    }
}
new Two();
?>
--EXPECTF--	
Fatal error: Uncaught exception 'AssertionException' with message 'assert(false)' in %sexpect_009.php:%d
Stack trace:
#0 %sexpect_009.php(%d): Two->__construct()
#1 {main}
  thrown in %sexpect_009.php on line %d
