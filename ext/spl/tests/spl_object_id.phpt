--TEST--
SPL: spl_object_id()
--FILE--
<?php

var_dump(spl_object_id(new stdClass));
$a = new stdClass();
var_dump(spl_object_id(new stdClass) === spl_object_id($a));

?>
--EXPECTF--
int(%d)
bool(false)
