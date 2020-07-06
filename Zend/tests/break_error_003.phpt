--TEST--
'break' error (not in the loop context)
--FILE--
<?php
function foo () {
    break;
}
?>
--EXPECTF--
Fatal error: break statement can only be used inside a loop or a switch statement in %s on line %d
