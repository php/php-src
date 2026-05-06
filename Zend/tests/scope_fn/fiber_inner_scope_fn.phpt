--TEST--
Scope fn declared inside a Fiber's wrapper closure: parent on fiber's vm_stack
--FILE--
<?php
$fiber = new Fiber(function () {
    $y = 0;
    $fn = fn() {
        $y++;
        Fiber::suspend("a");
        $y++;
        return $y;
    };
    return $fn();
});

var_dump($fiber->start());     // string(1) "a"
var_dump($fiber->resume());    // NULL (fiber returned 2, not via suspend)
var_dump($fiber->getReturn()); // int(2)
?>
--EXPECT--
string(1) "a"
NULL
int(2)
