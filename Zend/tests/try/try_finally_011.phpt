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
Fatal error: 'break' not in the 'loop' or 'switch' context in %stry_finally_011.php on line %d
