--TEST--
Assign result of by-value function to object property by-reference
--FILE--
<?php

function notRef() {
    return null;
}

$obj = new stdClass;
$obj->prop =& notRef();
var_dump($obj);

?>
--EXPECTF--
Notice: Only variables should be assigned by reference in %s on line %d
object(stdClass)#1 (1) {
  ["prop"]=>
  NULL
}
