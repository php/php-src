--TEST--
Bug #42838 (Wrong results in array_diff_uassoc())
--FILE--
<?php

function key_compare_func($a, $b)
{
	if ($a === $b) {
		return 0;
	}
	return ($a > $b)? 1:-1;
}

$array1 = array("a" => "green", "b" => "Brown", 'c' => 'blue', 0 => 'red');
$array2 = array("a" => "green", "b" => "Brown", 'c' => 'blue', 0 => 'red');

$result = array_diff_uassoc($array1, $array2, "key_compare_func");
print_r($result);

?>
--EXPECT--
Array
(
)
