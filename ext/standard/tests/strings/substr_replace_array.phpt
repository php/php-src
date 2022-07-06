--TEST--
substr_replace() function - array
--FILE--
<?php

$arr = array('abc' => 'llsskdkk','def' => 'llsskjkkdd', 4 => 'hello', 42 => 'world');
$newarr = substr_replace($arr, 'zzz', 0, -2);

print_r($newarr);

?>
--EXPECT--
Array
(
    [abc] => zzzkk
    [def] => zzzdd
    [4] => zzzlo
    [42] => zzzld
)
