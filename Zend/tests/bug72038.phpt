--TEST--
Bug #72038 (Function calls with values to a by-ref parameter don't always throw a notice)
--FILE--
<?php

test($foo = new stdClass);
var_dump($foo);
test($bar = 2);
var_dump($bar);
test($baz = &$bar);
var_dump($baz);

function test(&$param) {
        $param = 1;
}

?>
--EXPECTF--

Notice: Only variables should be passed by reference in %s on line %d
object(stdClass)#1 (0) {
}

Notice: Only variables should be passed by reference in %s on line %d
int(2)
int(1)

