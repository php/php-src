--TEST--
Bug #52534 (var_export array with negative key)
--FILE--
<?php

$aArray = array ( -1 => 'Hello');

var_export($aArray);

?>
--EXPECT--
array (
  -1 => 'Hello',
)
