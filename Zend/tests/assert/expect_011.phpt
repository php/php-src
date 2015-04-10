--TEST--
test overloaded __toString on custom exception
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php
class MyExpectations extends AssertionException {
    public function __toString() {
        return sprintf(
            "[Message]: %s", __CLASS__);
    }
}

class One {
    public function __construct() {
        assert(false, (string) new MyExpectations());
    }
}
class Two extends One {}

new Two();
?>
--EXPECTF--
Fatal error: Uncaught exception 'AssertionException' with message '[Message]: MyExpectations' in %sexpect_011.php:%d
Stack trace:
#0 %sexpect_011.php(%d): assert(false, '[Message]: MyEx...')
#1 %sexpect_011.php(%d): One->__construct()
#2 {main}
  thrown in %sexpect_011.php on line %d
