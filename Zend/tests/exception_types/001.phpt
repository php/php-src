--TEST--
Testing basic exception type compiler wrong type
--FILE--
<?php
function () throws int { }
?>
--EXPECTF--
Fatal error: Throws type must be an object in %s on line 2

