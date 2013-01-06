--TEST--
Bug #55135 (Array keys are no longer type casted in unset())
--FILE--
<?php
// This fails.
$array = array(1 => 2);
$a = "1";
unset($array[$a]);
print_r($array);

// Those works.
$array = array(1 => 2);
$a = 1;
unset($array[$a]);
print_r($array);

$array = array(1 => 2);
unset($array[1]);
print_r($array);

$array = array(1 => 2);
$a = 1;
unset($array["1"]);
print_r($array);
?>
--EXPECT--
Array
(
)
Array
(
)
Array
(
)
Array
(
)
