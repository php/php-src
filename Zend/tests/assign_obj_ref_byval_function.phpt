--TEST--
Assign result of by-value function to object property by-reference
--FILE--
<?php

function notRef() {
    return null;
}

$obj = new StdClass;
$obj->prop =& notRef();
var_dump($obj);

?>
--EXPECTF--
Notice: Only variables should be assigned by reference in %s on line %d
object(StdClass)#1 (1) {
  ["prop"]=>
  NULL
}
