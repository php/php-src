--TEST--
Generators can return without values
--FILE--
<?php

function gen() {
    yield;
    return;
}

function gen2() {
    yield;
    return null;
}

var_dump(gen());

var_dump(gen2());

?>
--EXPECTF--
object(Generator)#%d (0) {
}
object(Generator)#%d (0) {
}