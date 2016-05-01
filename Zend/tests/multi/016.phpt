--TEST--
intersection default values (fail)
--FILE--
<?php
function (IFoo & IBar $thing = 1) {};
?>
--EXPECTF--
Fatal error: Default value for intersection types can only be NULL in %s on line 2
