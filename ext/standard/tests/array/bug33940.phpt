--TEST--
Bug #33940 (array_map() fails to pass by reference when called recursively)
--INI--
error_reporting=4095
allow_call_time_pass_reference=1
--FILE--
<?php
function ref_map(&$item) {
    if(!is_array($item)) {
        $item = 1;
        return 2;
    } else {
        $ret = array_map('ref_map', &$item);
        return $ret;
    }
}

$a = array(array(0), 0);
$ret = array_map('ref_map', $a);
echo 'Array: '; print_r($a);
echo 'Return: '; print_r($ret);
$a = array(array(0), 0);
$ret = array_map('ref_map', &$a);
echo 'Array: '; print_r($a);
echo 'Return: '; print_r($ret);
?>
--EXPECT--
Array: Array
(
    [0] => Array
        (
            [0] => 0
        )

    [1] => 0
)
Return: Array
(
    [0] => Array
        (
            [0] => 2
        )

    [1] => 2
)
Array: Array
(
    [0] => Array
        (
            [0] => 1
        )

    [1] => 1
)
Return: Array
(
    [0] => Array
        (
            [0] => 2
        )

    [1] => 2
)
