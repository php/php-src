--TEST--
Bug #75786: segfault when using spread operator on generator passed by reference
--FILE--
<?php

function &gen($items) {
    foreach ($items as $key => &$value) {
        yield $key => $value;
    }
}

var_dump(...gen(['a', 'b', 'c']));

?>
--EXPECT--
string(1) "a"
string(1) "b"
string(1) "c"
