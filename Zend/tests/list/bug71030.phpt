--TEST--
Bug #71030: Self-assignment in list() may have inconsistent behavior
--FILE--
<?php

function test1() {
    $a = [1, 2];
    $c =& $a;
    list($c, $b) = $a;
    var_dump($a, $b);
}

function test2() {
    $a = [1, 2];
    $_a = "a";
    list($$_a, $b) = $a;
    var_dump($a, $b);
}

test1();
test2();

?>
--EXPECT--
int(1)
int(2)
int(1)
int(2)
