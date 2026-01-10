--TEST--
GH-13178: Packed array with last elements removed must reset iterator positions
--FILE--
<?php
$array = [0, 1, 2];
foreach ($array as &$value) {
    var_dump($value);
    if ($value === 2) {
        unset($array[2]);
        unset($array[1]);
        $array[1] = 3;
        $array[2] = 4;
    }
}
?>
--EXPECT--
int(0)
int(1)
int(2)
int(3)
int(4)
