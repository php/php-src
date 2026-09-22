--TEST--
PFA fuzz 005
--FILE--
<?php

function foo(int $day = 1, int $month = 1, int $yearh = 1, int $year = 2005) {
}

$foo = foo(month: 12, ...);

$bar = $foo(year: 2016, ...);

?>
==DONE==
--EXPECT--
==DONE==
