--TEST--
Bug #78182: Segmentation fault during by-reference property assignment
--FILE--
<?php
$varName = 'var';
$propName = 'prop';
$$varName->$propName =& $$varName;
var_dump($var);
?>
--EXPECTF--
Warning: Creating default object from empty value in %s on line %d
object(stdClass)#1 (1) {
  ["prop"]=>
  *RECURSION*
}
