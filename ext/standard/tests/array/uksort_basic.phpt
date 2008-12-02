--TEST--
Test uksort(): basic functionality
--FILE--
<?php
/*
* proto bool uksort ( array &$array, callback $cmp_function )
* Function is implemented in ext/standard/array.c
*/
function cmp($a, $b) {
    if ($a == $b) {
        return 0;
    }
    return ($a < $b) ? -1 : 1;
}
$a = array(3, 2, 5, 6, 1);
uasort($a, "cmp");
foreach($a as $key => $value) {
    echo "$key: $value\n";
}
?>
--EXPECT--
4: 1
1: 2
0: 3
2: 5
3: 6