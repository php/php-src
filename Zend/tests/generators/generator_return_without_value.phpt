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

function gen3() {
	return;
    yield;
}

function gen4() {
	return;
    yield;
}

var_dump(gen());

var_dump(gen2());

var_dump(gen3());

var_dump(gen4());

?>
--EXPECTF--
object(Generator)#%d (0) {
}
object(Generator)#%d (0) {
}
object(Generator)#%d (0) {
}
object(Generator)#%d (0) {
}
