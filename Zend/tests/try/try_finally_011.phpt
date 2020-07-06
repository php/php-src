--TEST--
Try finally (segfault with empty break)
--FILE--
<?php
function foo () {
    try {
        break;
    } finally {
    }
}

foo();
?>
--EXPECTF--
Fatal error: break statement can only be used inside a loop or a switch statement in %s on line %d
