--TEST--
Fix UMR in array_user_key_compare (MOPB24)
--FILE--
<?php
$arr = array("A" => 1, "B" => 1);

function array_compare(&$key1, &$key2)
  {
    $GLOBALS['a'] = &$key2;
    unset($key2);
    return 1;
  }

uksort($arr, "array_compare");
var_dump($a);

?>
--EXPECTF--
Warning: array_compare(): Argument #1 ($key1) must be passed by reference, value given in %s on line %d

Warning: array_compare(): Argument #2 ($key2) must be passed by reference, value given in %s on line %d
string(1) "B"
