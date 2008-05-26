--TEST--
errmsg: instanceof expects an object instance, constant given
--FILE--
<?php

var_dump("abc" instanceof stdclass);

echo "Done\n";
?>
--EXPECTF--
Fatal error: instanceof expects an object instance, constant given in %s on line %d
