--TEST--
PFA in constexpr: cache retained until shutdown
--FILE--
<?php

function g($a, $b) {
    return $a + $b;
}

$f = function ($x = g(1, ?)) {
    return $x;
};

var_dump($f()(2));
$p = $f(?);
var_dump($p(5));

?>
--EXPECT--
int(3)
int(5)
