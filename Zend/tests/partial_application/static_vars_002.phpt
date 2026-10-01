--TEST--
PFA static variables are shared (002)
--FILE--
<?php
$closure = function ($a) {
    static $var = 0;

    ++$var;

    return $var;
};

var_dump($closure(new stdClass));

$foo = $closure(new stdClass, ...);
$closure = null;

var_dump($foo());
?>
--EXPECT--
int(1)
int(2)
