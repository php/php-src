--TEST--
GH-22490: Null pointer dereference compiling a pipe on the lhs of ??=
--FILE--
<?php
$calls = 0;
function unset_prop($x) {
    global $calls;
    $calls++;
    return new stdClass;
}
function set_prop($x) {
    global $calls;
    $calls++;
    $o = new stdClass;
    $o->p = 7;
    return $o;
}

echo "property unset, assigns and returns default: ";
var_dump((1 |> unset_prop(...))->p ??= 99);
echo "pipe evaluated once: ";
var_dump($calls);

$calls = 0;
echo "property set, returns existing value: ";
var_dump((1 |> set_prop(...))->p ??= 99);
echo "pipe evaluated once: ";
var_dump($calls);

function new_array($x) {
    global $calls;
    $calls++;
    return [];
}

$calls = 0;
echo "dim target on a pipe result: ";
var_dump((1 |> new_array(...))[0] ??= 42);
echo "pipe evaluated once: ";
var_dump($calls);

function identity($x) {
    return $x;
}

$calls = 0;
echo "nested pipe on the lhs: ";
var_dump((1 |> identity(...) |> unset_prop(...))->p ??= 5);
echo "pipe evaluated once: ";
var_dump($calls);
?>
--EXPECT--
property unset, assigns and returns default: int(99)
pipe evaluated once: int(1)
property set, returns existing value: int(7)
pipe evaluated once: int(1)
dim target on a pipe result: int(42)
pipe evaluated once: int(1)
nested pipe on the lhs: int(5)
pipe evaluated once: int(1)
