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
$a = array(3=>0, 2=>1, 5=>2, 6=>3, 1=>4);
uksort($a, "cmp");
foreach($a as $key => $value) {
    echo "$key: $value\n";
}
?>
--EXPECT--
1: 4
2: 1
3: 0
5: 2
6: 3