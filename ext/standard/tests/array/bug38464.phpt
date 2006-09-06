--TEST--
Bug #38464 (array_count_values() mishandles numeric strings)
--FILE--
<?php
$array = array('-000', ' 001', 1, ' 123', '+123');
var_dump(array_count_values($array));
?>
--EXPECT--	
array(5) {
  ["-000"]=>
  int(1)
  [" 001"]=>
  int(1)
  [1]=>
  int(1)
  [" 123"]=>
  int(1)
  ["+123"]=>
  int(1)
}
