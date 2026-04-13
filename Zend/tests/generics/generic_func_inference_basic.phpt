--TEST--
Generic function: lazy type inference from call arguments
--DESCRIPTION--
Tests the lazy inference path in zend_check_type_slow() which infers
generic type params from call arguments on first type check.
Note: generic function inference only enforces the return type — it
does not retroactively validate other parameters with the same T.
--FILE--
<?php
declare(strict_types=1);

// 1. Basic: infer T=int, enforce on return
function identity<T>(T $v): T {
    return $v;
}

echo "1. " . identity(42) . "\n";
echo "1. " . identity("hello") . "\n";

// 2. Wrong return type (array is not coercible to string)
function broken<T>(T $v): T {
    return [1, 2, 3];
}

try {
    broken("hello");
    echo "FAIL\n";
} catch (TypeError $e) {
    echo "2. TypeError OK\n";
}

// 3. Multiple type params: infer A and B independently
function make_pair<A, B>(A $a, B $b): array {
    return [$a, $b];
}

$p = make_pair(42, "hello");
echo "3. " . $p[0] . " " . $p[1] . "\n";

// 4. Type param only in return, can't infer — no check
function make<T>(): T {
    return 42;
}

echo "4. " . make() . "\n"; // no error, T can't be inferred

// 5. Param with constraint
function count_items<T: Countable>(T $v): int {
    return count($v);
}

echo "5. " . count_items(new ArrayObject([1, 2, 3])) . "\n";

// 6. Boolean inference
function check<T>(T $v): T {
    return $v;
}

echo "6. " . (check(true) ? "true" : "false") . "\n";
echo "6. " . (check(false) ? "true" : "false") . "\n";

// 7. Object inference
class Foo { public string $name = "Foo"; }

function get_name<T>(T $obj): string {
    if ($obj instanceof Foo) return $obj->name;
    return "unknown";
}

echo "7. " . get_name(new Foo()) . "\n";

// 8. Null inference (IS_NULL)
function nullable<T>(T $v): T {
    return $v;
}

var_dump(nullable(null));

echo "Done.\n";
?>
--EXPECT--
1. 42
1. hello
2. TypeError OK
3. 42 hello
4. 42
5. 3
6. true
6. false
7. Foo
NULL
Done.
