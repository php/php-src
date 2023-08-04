--TEST--
May not goto into match expression block
--FILE--
<?php
goto in;
var_dump(match (1) {
    1 => {
in:
        <- 42;
    },
});
?>
--EXPECTF--
Fatal error: 'goto' into loop or switch statement is disallowed in %s on line %d
