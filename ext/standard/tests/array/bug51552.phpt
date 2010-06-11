--TEST--
Bug #51552 (debug_backtrace() causes segmentation fault and/or memory issues)
--FILE--
<?php
function walk($element, $key, $p) {
    $backtrace = debug_backtrace();
    echo "$element\n";
}

$a = array(1,2,3,4,5,6,7,8,9,10);
array_walk($a, 'walk', 'testthis');
?>
--EXPECT--
1
2
3
4
5
6
7
8
9
10
