--TEST--
PFA static variables are shared (003)
--FILE--
<?php
$closure = function ($a) {
    static $var = 0;

    ++$var;

    return $var;
};

var_dump($closure(new stdClass));

$foo = $closure(?);
$closure = null;

$bar = $foo(?);
$foo = null;

var_dump($bar(new stdClass));
?>
--EXPECT--
int(1)
int(2)
