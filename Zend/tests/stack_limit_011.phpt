--TEST--
Stack limit 011 - Stack limit exhaustion in shutdown function
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(zend_test_zend_call_stack_get());

register_shutdown_function(function () {
    new Test1;
});

class Test1 {
    public function __destruct() {
        new Test1;
    }
}

new Test1;

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

Fatal error: Stack overflow. Infinite recursion? in %s on line 11

Fatal error: Stack overflow. Infinite recursion? in %s on line 11
