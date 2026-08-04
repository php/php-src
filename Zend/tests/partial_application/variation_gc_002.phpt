--TEST--
PFA variation: GC (002)
--FILE--
<?php
$obj = new stdClass;
$obj->prop = var_dump($obj, ?);

echo "OK";
?>
--EXPECT--
OK
