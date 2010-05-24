--TEST--
Parameter type hint - scalar type hints, default value
--FILE--
<?php

function bar1(scalar $param = null) {}
function bar2(scalar $param = 123) {}
function bar3(scalar $param = false) {}
function bar4(scalar $param = 988.99) {}
function bar5(scalar $param = "1233") {}
function bar6(scalar $param = array(1,2,3)) {}

?>
--EXPECTF--
Fatal error: Default value for parameters with scalar type hint can only be scalar or NULL in %s on line 8
