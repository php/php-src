--TEST--
GH-18262 004 (Assertion failure Zend/zend_vm_execute.h JIT)
--FILE--
<?php
class B {
    public function __construct(
        public $throw,
    ) { }
    public function __get($name) {
        return $this->throw === '1' ? 'str' : 1;
    }
    public function __destruct() {
        if ($this->throw === '1') {
            throw new Exception(__METHOD__);
        }
    }
}

$tests = [
    '0',
    '1',
];

foreach ($tests as $test) {
    // Second iteration exits, and free op1 throws
    var_dump((new B($test))->b);
}
?>
--EXPECTF--
int(1)

Fatal error: Uncaught Exception: B::__destruct in %s:%d
Stack trace:
#0 %s(%d): B->__destruct()
#1 {main}
  thrown in %s on line %d
