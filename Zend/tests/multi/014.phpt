--TEST--
union default values (fail)
--FILE--
<?php
function (IFoo | IBar $thing = 1) {};
?>
--EXPECTF--
Fatal error: Default type integer does not match allowed types null for parameter 1 in %s on line 2
