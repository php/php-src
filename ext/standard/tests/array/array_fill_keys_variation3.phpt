--TEST--
Test array_fill_keys() function : variation of parameter
--FILE--
<?php
/* Testing with unexpected argument types */

echo "*** Testing array_fill_keys() : parameter variations ***\n";

$fp = fopen(__FILE__, "r");
$array = array("one", "two");

echo "\n-- Testing array_fill_keys() function with unusual second arguments --\n";
var_dump( array_fill_keys($array, $fp) );

fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing array_fill_keys() : parameter variations ***

-- Testing array_fill_keys() function with unusual second arguments --
array(2) {
  ["one"]=>
  resource(%d) of type (stream)
  ["two"]=>
  resource(%d) of type (stream)
}
Done
