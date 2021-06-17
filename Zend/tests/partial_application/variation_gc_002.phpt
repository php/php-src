--TEST--
Partial application variation GC
--FILE--
<?php
$obj = new stdClass;
$obj->prop = var_dump($obj, ?);

echo "OK";
?>
--EXPECT--
OK
