--TEST--
Tests passing default as a named argument
--FILE--
<?php

json_encode([], depth: $D = default);
var_dump($D);

function F($V = 'Alfa', $V2 = 1) {
    var_dump($V2);
}
F(V2: default + 1);
?>
--EXPECT--
int(512)
int(2)
