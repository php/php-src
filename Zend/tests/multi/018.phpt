--TEST--
intersection disallow void
--FILE--
<?php
function () : Foo and void {};
?>
--EXPECTF--
Fatal error: void is not allowed in intersections in %s on line 2
