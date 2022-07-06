--TEST--
Array object clobbering by user error handler
--FILE--
<?php
class A implements ArrayAccess {
    public function &offsetGet($n) {
    }
    public function offsetSet($n, $v) {
    }
    public function offsetUnset($n) {
    }
    public function offsetExists($n) {
    }
}

set_error_handler(function () {
    $GLOBALS['a'] = null;
});

$a = new A;
$a[$c];
?>
DONE
--EXPECT--
DONE
