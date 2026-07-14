--TEST--
Scope function runs inside a Fiber and is fully resumed before parent exits
--FILE--
<?php
function outer() {
    $x = 0;
    $fn = fn() {
        $x = 1;
        Fiber::suspend("a");
        $x = 2;
        Fiber::suspend("b");
        $x = 3;
    };
    $fiber = new Fiber($fn);
    var_dump($fiber->start());
    var_dump($x);
    var_dump($fiber->resume());
    var_dump($x);
    $fiber->resume();
    var_dump($x);
    /* Fiber finished -> only this scope holds $fn, no escape. */
}
outer();
echo "ok\n";
?>
--EXPECT--
string(1) "a"
int(1)
string(1) "b"
int(2)
int(3)
ok
