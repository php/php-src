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
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    ["function"]=>
    string(9) "{closure}"
    ["args"]=>
    array(0) {
    }
  }
}
