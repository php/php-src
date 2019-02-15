--TEST--
Bug #77395 (segfault about array_multisort)
--FILE--
<?php
function error_handle($level, $message, $file = '', $line = 0){
	$a = [1,2,3];
	$b = [3,2,1];
	echo $message;
	array_multisort($a, SORT_ASC, $b); // if comment this line, no segfault happen
}
set_error_handler('error_handle');
$data = [['aa'=> 'bb',], ['aa'=> 'bb',],];
array_multisort(array_column($data, 'bb'),SORT_DESC, $data); // PHP Warning error 
?>
--EXPECT--
array_multisort(): Array sizes are inconsistent
