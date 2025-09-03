--TEST--
GH-19679: zend_ssa_range_widening does not converge
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 8) {
  die('skip output depends PHP_INT_SIZE=8');
}
?>
--FILE--
<?php
function test() {
    $a = PHP_INT_MIN+1;
    $b = 0;
    while ($b++ < 3) {
        $a = (int) ($a-- - $b - 1);
    }
    return $a;
}
var_dump(test() == PHP_INT_MIN);
?>
--EXPECT--
bool(true)
