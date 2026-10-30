--TEST--
GH-16230: Segfault on debug_backtrace() inside _ZendTestFiber
--EXTENSIONS--
zend_test
--FILE--
<?php
$test = new _ZendTestFiber(function (): void {
    var_dump(debug_backtrace());
});
$test->start();
?>
--EXPECTF--
array(1) {
  [0]=>
  array(2) {
    ["function"]=>
    string(%d) "{closure:%s:%d}"
    ["args"]=>
    array(0) {
    }
  }
}
