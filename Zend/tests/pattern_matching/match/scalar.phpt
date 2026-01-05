--TEST--
Scalar pattern matching
--FILE--
<?php

var_dump(match (2) {
    is 1 => 1,
    is 2 => 2,
    is 3 => 3,
});

?>
--EXPECT--
int(2)
