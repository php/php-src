--TEST--
Match expression block may use break if block is not escaped
--FILE--
<?php
var_dump(match (1) {
    1 => {
        foreach ([1, 2, 3] as $value) {
            echo $value, "\n";
            break;
        }
        <- 42;
    },
});
?>
--EXPECT--
1
int(42)
