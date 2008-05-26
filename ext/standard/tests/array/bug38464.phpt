--TEST--
Bug #38464 (array_count_values() mishandles numeric strings)
--FILE--
<?php
$array = array('-000', ' 001', 1, ' 123', '+123');
var_dump(array_count_values($array));
?>
--EXPECT--
array(5) {
  [u"-000"]=>
  int(1)
  [u" 001"]=>
  int(1)
  [1]=>
  int(1)
  [u" 123"]=>
  int(1)
  [u"+123"]=>
  int(1)
}
