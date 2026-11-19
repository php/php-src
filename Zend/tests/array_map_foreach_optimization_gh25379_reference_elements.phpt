--TEST--
GH-25379: array_map() foreach optimization preserves by-ref callbacks with reference elements
--FILE--
<?php
function known(&$x) {
    return ++$x;
}
function byValue($x) {
    return ++$x;
}
class ByRefMethod {
    public static function change(&$x) {
        return ++$x;
    }
}
function test(string $name, Closure $c) {
    echo $name, ':', PHP_EOL;
    try {
        echo json_encode($c()), PHP_EOL;
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}
test('known callee', function () {
    $a = [1, 2];
    $r = &$a[0];
    $res = array_map(known(...), $a);
    return [$res, $a];
});
test('callee declared later', function () {
    $a = [1, 2];
    $r = &$a[0];
    $res = array_map(later(...), $a);
    return [$res, $a];
});
test('static method', function () {
    $a = [1, 2];
    $r = &$a[0];
    $res = array_map(ByRefMethod::change(...), $a);
    return [$res, $a];
});
test('by-value callee', function () {
    $a = [1, 2];
    $r = &$a[0];
    $res = array_map(byValue(...), $a);
    return [$res, $a];
});
function later(&$x) {
    return ++$x;
}
?>
--EXPECTF--
known callee:

Warning: known(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
[[2,3],[2,2]]
callee declared later:

Warning: later(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
[[2,3],[2,2]]
static method:

Warning: ByRefMethod::change(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
[[2,3],[2,2]]
by-value callee:
[[2,3],[1,2]]
