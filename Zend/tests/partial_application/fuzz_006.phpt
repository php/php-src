--TEST--
PFA fuzz 006
--FILE--
<?php

function foo(int $day = 1, int $month = 1, int $year = 2005) {
    var_dump($day);
}

$foo = foo(month: 12, ...);

$foo(year: 2025);

?>
--EXPECT--
int(1)
