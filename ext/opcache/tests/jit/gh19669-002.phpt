--TEST--
GH-19669 002: assertion failure zend_jit_trace_type_to_info_ex
--CREDITS--
YuanchengJiang
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 8) {
  die('skip output depends PHP_INT_SIZE=8');
}
?>
--FILE--
<?php
function test() {
    $a = PHP_INT_MIN;
    $b = -1;
    while ($b++ < 2) {
        $a = (int) (--$a + $a - $b);
    }
    return $a;
}
var_dump(test());
?>
--EXPECT--
int(-10)
