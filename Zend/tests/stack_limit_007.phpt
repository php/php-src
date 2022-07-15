--TEST--
Stack overflow 007 - Shutdown function
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(zend_test_zend_call_stack_get());

register_shutdown_function(function () {
    var_dump(error_get_last()['message']);

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

Fatal error: Stack overflow. Infinite recursion? in %s on line %d
string(%d) "Stack overflow. Infinite recursion?"
