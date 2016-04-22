--TEST--
union default values (fail)
--FILE--
<?php
function (IFoo | IBar $thing = 1) {};
?>
--EXPECTF--
Fatal error: Default value for union types can only be NULL in %s on line 2
