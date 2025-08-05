--TEST--
PFA fuzz 007
--FILE--
<?php

function foo(int $day = 1, int $month = UNDEFINED, int $year = 2005) {
}

$foo = foo(year: 2006, ...);

try {
    $foo(2);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Undefined constant "UNDEFINED"
