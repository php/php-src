--TEST--
GH-24668 (zend_set_local_var() leaks the previous value of an existing CV)
--EXTENSIONS--
zend_test
--FILE--
<?php

class Dtor {
    public function __destruct() {
        echo "dtor\n";
        $GLOBALS['trace'] = debug_backtrace();
    }
}

function test($a) {
    var_dump(zend_test_set_local_var('a', 42));
    var_dump($a);
}

test([new Dtor]);
var_dump($trace[2]['args']);

?>
--EXPECT--
dtor
bool(true)
int(42)
array(1) {
  [0]=>
  int(42)
}
