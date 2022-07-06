--TEST--
Bug #34518 (Unset doesn't separate container in CV)
--FILE--
<?php
$arr = array(1,2,3);
$arr["foo"] = array(4,5,6);
$copy = $arr;

unset($copy["foo"][0]);
print_r($arr);
print_r($copy);
?>
--EXPECT--
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [foo] => Array
        (
            [0] => 4
            [1] => 5
            [2] => 6
        )

)
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [foo] => Array
        (
            [1] => 5
            [2] => 6
        )

)
