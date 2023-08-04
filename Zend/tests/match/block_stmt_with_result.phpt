--TEST--
Match statement block must not return a value
--FILE--
<?php
match (1) {
    1 => { <- 42; },
};
?>
--EXPECTF--
Fatal error: Blocks of match expression with unused result must not return a value in %s on line %d
