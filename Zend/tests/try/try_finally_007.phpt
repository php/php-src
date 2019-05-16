--TEST--
Try finally (with goto previous label)
--FILE--
<?php
function foo () {
    try {
label:
        echo "label";
        try {
        } finally {
            goto label;
            echo "dummy";
        }
    } catch (Exception $e) {
    } finally {
    }
}

foo();
?>
--EXPECTF--
Fatal error: jump out of a finally block is disallowed in %stry_finally_007.php on line %d
