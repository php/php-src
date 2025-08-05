--TEST--
PFA static variables are shared (002)
--FILE--
<?php
$closure = function ($a) {
    static $var = 0;

    ++$var;

    return $var;
};

$closure(new stdClass);

$foo = $closure(new stdClass, ...);
$closure = null;

if ($foo() == 2) {
    echo "OK";
}
?>
--EXPECT--
OK
