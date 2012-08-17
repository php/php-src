--TEST--
Finally with jmp (do while)
--FILE--
<?php
function foo () {
    do {
        try {
            try {
            } finally {
                goto label;
                echo "dummy";
            }
        } catch (Exception $e) {
        } finally {
        }
    } while (0);
label:
        echo "label"; 
}

foo();
?>
--EXPECTF--
Fatal error: jump out of a finally block is disallowed in %stry_finally_008.php on line %d
