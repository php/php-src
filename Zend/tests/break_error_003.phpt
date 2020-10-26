--TEST--
'break' error (not in the loop context)
--FILE--
<?php
function foo () {
    break;
}
?>
--EXPECTF--
Fatal error: The break statement can only be used inside a loop or a switch in %s on line %d
