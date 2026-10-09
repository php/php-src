--TEST--
List assignment from an array literal: undefined variables
--FILE--
<?php

function f() {
    [$a, $b] = [$u0, $u1];
    var_dump($a, $b);

    [$a,
     $b] = [
        $u2,
        $u3];
    var_dump($a, $b);
}
f();

?>
--EXPECTF--

Warning: Undefined variable $u0 in %s on line 4

Warning: Undefined variable $u1 in %s on line 4
NULL
NULL

Warning: Undefined variable $u2 in %s on line 9

Warning: Undefined variable $u3 in %s on line 10
NULL
NULL
