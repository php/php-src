--TEST--
May not goto into match statement block
--FILE--
<?php
goto in;
match (1) {
    1 => {
in:
        echo "Inside match block\n";
    },
};
?>
--EXPECTF--
Fatal error: 'goto' into loop or switch statement is disallowed in %s on line %d
