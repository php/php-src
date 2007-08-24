--TEST--
039: Constant declaration
--FILE--
<?php
const A = "ok";
const B = A;
const C = array("ok");
const D = array(B);
echo A . "\n";
echo B . "\n";
print_r(C);
print_r(D);
--EXPECT--
ok
ok
Array
(
    [0] => ok
)
Array
(
    [0] => ok
)
