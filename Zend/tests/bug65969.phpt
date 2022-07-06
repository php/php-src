--TEST--
Bug #65969 (Chain assignment with T_LIST failure)
--FILE--
<?php
$obj = new stdClass;
list($a,$b) = $obj->prop = [1,2];
var_dump($a,$b);
--EXPECT--
int(1)
int(2)
