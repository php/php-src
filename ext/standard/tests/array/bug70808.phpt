--TEST--
Bug #70808 (array_merge_recursive corrupts memory of unset items)
--FILE--
<?php

$arr1 = array("key" => array(0, 1));
$arr2 = array("key" => array(2));

unset($arr1["key"][1]);

$result = array_merge_recursive($arr1, $arr2);
print_r($result);
?>
--EXPECT--
Array
(
    [key] => Array
        (
            [0] => 0
            [2] => 2
        )

)
