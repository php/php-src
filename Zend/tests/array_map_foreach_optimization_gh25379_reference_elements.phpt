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
class ParentByValue {
    public static function m($x) {
        return $x + 1;
    }
}
trait ByRefTrait {
    public static function m($y, &...$x) {
        return ++$x[0];
    }
}
class DeclaredLater extends ParentByValue {
    public static function positional() {
        $a = [1, 2];
        $r = &$a[0];
        $res = array_map(self::m(0, ?), $a);
        return [$res, $a];
    }
    public static function named() {
        $a = [1, 2];
        $r = &$a[0];
        $res = array_map(self::m(0, x: ?), $a);
        return [$res, $a];
    }
    public static function byClassName() {
        $a = [1, 2];
        $r = &$a[0];
        $res = array_map(DeclaredLater::m(0, ?), $a);
        return [$res, $a];
    }
    public static function m($y, &$x = null) {
        return ++$x;
    }
}
class FromTrait extends ParentByValue {
    use ByRefTrait;

    public static function variadic() {
        $a = [1, 2];
        $r = &$a[0];
        $res = array_map(self::m(0, ?), $a);
        return [$res, $a];
    }
}
class ChildOfFromTrait extends FromTrait {
    public static function viaParent() {
        $a = [1, 2];
        $r = &$a[0];
        $res = array_map(parent::m(0, ?), $a);
        return [$res, $a];
    }
}
function preboundRef(&$a, $b) {
    return $b + 1;
}
class NonStaticMethod {
    public function inst($x) {
        return $x + 1;
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
test('declared later, positional', DeclaredLater::positional(...));
test('declared later, named', DeclaredLater::named(...));
test('declared later, class name', DeclaredLater::byClassName(...));
test('from trait, variadic', FromTrait::variadic(...));
test('parent from trait', ChildOfFromTrait::viaParent(...));
test('prebound by-ref literal on empty array', fn () => array_map(preboundRef(0, ?), []));
test('non-static method on empty array', fn () => array_map(NonStaticMethod::inst(...), []));
test('parent in free function', fn () => eval('function _gh25379_free() { return array_map(parent::m(...), [1]); }'));
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
declared later, positional:

Warning: DeclaredLater::{closure:pfa:DeclaredLater::positional():%d}(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
[[2,3],[2,2]]
declared later, named:

Warning: DeclaredLater::{closure:pfa:DeclaredLater::named():%d}(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
[[2,3],[2,2]]
declared later, class name:

Warning: DeclaredLater::{closure:pfa:DeclaredLater::byClassName():%d}(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
[[2,3],[2,2]]
from trait, variadic:

Warning: FromTrait::{closure:pfa:FromTrait::variadic():%d}(): Argument #1 ($x0) must be passed by reference, value given in %s on line %d
[[2,3],[2,2]]
parent from trait:

Warning: FromTrait::{closure:pfa:ChildOfFromTrait::viaParent():%d}(): Argument #1 ($x0) must be passed by reference, value given in %s on line %d
[[2,3],[2,2]]
prebound by-ref literal on empty array:
Error: preboundRef(): Argument #1 ($a) could not be passed by reference
non-static method on empty array:
Error: Non-static method NonStaticMethod::inst() cannot be called statically
parent in free function:

Fatal error: Cannot use "parent" when no class scope is active in %s(%d) : eval()'d code on line 1
