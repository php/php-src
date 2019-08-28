--TEST--
SPL: spl_object_id()
--FILE--
<?php

var_dump(spl_object_id(new StdClass));
$a = new StdClass();
var_dump(spl_object_id(new StdClass) === spl_object_id($a));

?>
--EXPECTF--
int(%d)
bool(false)
