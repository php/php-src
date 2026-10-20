--TEST--
PFA static variables are shared (001)
--FILE--
<?php
function foo($a) {
    static $var = 0;

    ++$var;

    return $var;
}

var_dump(foo(new stdClass));

$foo = foo(new stdClass, ...);

var_dump($foo());
?>
--EXPECT--
int(1)
int(2)
