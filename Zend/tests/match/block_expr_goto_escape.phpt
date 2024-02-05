--TEST--
Match expression block must not use goto
--FILE--
<?php
var_dump(match (1) {
    1 => {
        goto after;
    },
});
after:
?>
--EXPECTF--
Fatal error: Match expression whose result is used must not contain return, break, continue or goto in %s on line %d
