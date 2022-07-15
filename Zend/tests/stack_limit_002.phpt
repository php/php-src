--TEST--
Stack overflow 002 - fibers
--EXTENSIONS--
zend_test
--INI--
fiber.stack_size=512k
--FILE--
<?php

var_dump(zend_test_zend_call_stack_get());

class Test1 {
    public function __destruct() {
        new Test1;
    }
}

$fiber = new Fiber(function (): void {
    new Test1;
});

$fiber->start();

?>
--EXPECTF--
array(4) {
  ["base"]=>
  string(%d) "0x%x"
  ["max_useable_size"]=>
  string(%d) "0x%x"
  ["max_size"]=>
  string(%d) "0x%x"
  ["position"]=>
  string(%d) "0x%x"
}

Fatal error: Stack overflow. Infinite recursion? in %s on line 7
