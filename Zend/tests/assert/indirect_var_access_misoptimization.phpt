--TEST--
Misoptimization when variable is modified by assert()
--INI--
zend.assertions=1
--FILE--
<?php

function test() {
    $i = 0;
    assert('$i = new stdClass');
    $i += 1;
    var_dump($i);
}
test();

?>
--EXPECTF--
Notice: Object of class stdClass could not be converted to int in %s on line %d
int(2)
