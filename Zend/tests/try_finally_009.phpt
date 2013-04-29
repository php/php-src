--TEST--
Try finally (with for continue)
--FILE--
<?php
function foo () {
    for($i = 0; $i < 5; $i++) {
        do {
            try {
                try {
                } finally {
                }
            } catch (Exception $e) {
            } finally {
              continue;
            }
        } while (0);
    }
}

foo();
?>
--EXPECTF--
Fatal error: jump out of a finally block is disallowed in %stry_finally_009.php on line %d
