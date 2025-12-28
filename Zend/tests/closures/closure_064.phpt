--TEST--
The closure name includes the source location
--FILE--
<?php
function get_closure3($c) {
    return function () use ($c) {
        return $c();
    };
}
function throws() {
    throw new \Exception();
}
$throws = throws(...);
$closure1 = function() use ($throws) { $throws(); };
$closure2 = fn () => $closure1();
$closure3 = get_closure3($closure2);
\array_map(
    function ($item) use ($closure3) { $closure3(); },
    [1]
);
?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:8
Stack trace:
#0 %s(11): throws()
#1 %s(12): {closure:%s:11}()
#2 %s(4): {closure:%s:12}()
#3 %s(15): {closure:%s:3}()
#4 [internal function]: {closure:%s:15}(1)
#5 %s(14): array_map(Object(Closure), Array)
#6 {main}
  thrown in %s on line 8
