--TEST--
intersection disallow void
--FILE--
<?php
function () : Foo & void {};
?>
--EXPECTF--
Fatal error: void is not allowed in intersections in %s on line 2
