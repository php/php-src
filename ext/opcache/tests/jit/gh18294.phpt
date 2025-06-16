--TEST--
GH-18294 (assertion failure zend_jit_ir.c)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1152
opcache.jit_hot_func=1
opcache.jit_hot_side_exit=1
--FILE--
<?php
$a = [2147483647,2147483647,2147483647,3,0,0,32,2147483584,127];
print_r(@bitwise_small_split($a));
function bitwise_small_split($val) {
    $split = 8;
    $len = count($val);
    while ($i != $len) {
        if (!$overflow) {
            $overflow = $split <= $remaining ? 0 : $split - $remaining;
        } elseif (++$i != $len) {
            $fusion[$i] >>= $overflow;
        }
    }
    return $fusion;
}
?>
--EXPECT--
Array
(
    [1] => 0
    [2] => 0
    [3] => 0
    [4] => 0
    [5] => 0
    [6] => 0
    [7] => 0
    [8] => 0
)
